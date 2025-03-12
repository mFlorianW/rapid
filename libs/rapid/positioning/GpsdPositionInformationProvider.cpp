// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsdPositionInformationProvider.hpp"
#include <cmath>
#include <fcntl.h>
#include <filesystem>
#include <gps.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Rapid::Positioning
{
constexpr auto FIFONAME = "/tmp/rapid_headless_gps_2";

namespace Private
{

struct FifoData
{
    float longitude{0.0f};
    float latitude{0.0f};
    double velocity{0.0};
    long timeSec{0};
    long long timeNanoSec{0};
    std::uint8_t satellites{0};
    GpsFixMode gpsFix{GpsFixMode::NoFix};
};

class GpsdProvider
{
public:
    GpsdProvider() = default;
    ~GpsdProvider()
    {
        gps_stream(&mGpsd, WATCH_DISABLE, nullptr);
        if (mGpsProvider.joinable()) {
            mGpsProvider.join();
        }
        gps_close(&mGpsd);
        close(mWriteFifo);
    }

    GpsdProvider(GpsdProvider const&) = delete;
    GpsdProvider& operator=(GpsdProvider const&) = delete;
    GpsdProvider(GpsdProvider&&) noexcept = delete;
    GpsdProvider& operator=(GpsdProvider&&) noexcept = delete;

    void start()
    {
        mGpsProvider = std::thread{&GpsdProvider::run, this};
        mRunning = true;
    }

    void stop()
    {
        mRunning = false;
    }

private:
    void run()
    {
        mWriteFifo = open(FIFONAME, O_WRONLY | O_NONBLOCK);
        if (mWriteFifo < 0) {
            SPDLOG_ERROR("Failed to open write buffer. ErrorCode: {}, Error: {}", errno, strerror(errno));
        }
        auto error = gps_open("localhost", "2947", &mGpsd);
        if (error < 0) {
            SPDLOG_ERROR("Failed to connected GPSD. Error: {}", gps_errstr(error));
            return;
        }
        gps_stream(&mGpsd, WATCH_ENABLE | WATCH_JSON, nullptr);
        auto buffer = Private::FifoData{};
        while (gps_waiting(&mGpsd, 5000000) and mRunning) {
            if (-1 == gps_read(&mGpsd, nullptr, 0)) {
                printf("Read error.  Bye, bye\n");
                break;
            }
            if (MODE_SET != (MODE_SET & mGpsd.set)) {
                // did not even get mode, nothing to see here
                continue;
            }
            if (0 > mGpsd.fix.mode or 4 <= mGpsd.fix.mode) {
                mGpsd.fix.mode = 0;
            }
            buffer.gpsFix = convertToFixMode(mGpsd.fix.mode);
            if (TIME_SET == (TIME_SET & mGpsd.set)) {
                buffer.timeSec = mGpsd.fix.time.tv_sec;
                buffer.timeNanoSec = mGpsd.fix.time.tv_nsec;
            }
            if (std::isfinite(mGpsd.fix.latitude) && std::isfinite(mGpsd.fix.longitude)) {
                buffer.latitude = static_cast<float>(mGpsd.fix.latitude);
                buffer.longitude = static_cast<float>(mGpsd.fix.longitude);
            }
            if (std::isfinite(mGpsd.fix.speed)) {
                buffer.velocity = mGpsd.fix.speed;
            }
            if (mGpsd.satellites_used > 0) {
                buffer.satellites = mGpsd.satellites_used;
            }
            auto bytesWritten = write(mWriteFifo, &buffer, sizeof(buffer));
            if (bytesWritten < 0 or bytesWritten < static_cast<ssize_t>(sizeof(buffer))) {
                SPDLOG_ERROR("Failed to write to FIFO. ErrorCode: {}, Error: {}", errno, strerror(errno));
            }
        }
    }

    GpsFixMode convertToFixMode(int mode)
    {
        switch (mode) {
        case 1:
            return GpsFixMode::NoFix;
        case 2:
            return GpsFixMode::Fix2d;
        case 3:
            return GpsFixMode::Fix3d;
        default:
            return GpsFixMode::NoFix;
        }
    }

    gps_data_t mGpsd{};
    std::thread mGpsProvider;
    std::atomic_bool mRunning;
    int mWriteFifo{-1};
};

} // namespace Private

GpsdPositionInformationProvider::GpsdPositionInformationProvider()
    : mProvider{std::make_unique<Private::GpsdProvider>()}
    , mReadNotifier{System::FdNotifierType::Read}
{
    if (std::filesystem::exists(FIFONAME) and not std::filesystem::remove(FIFONAME)) {
        SPDLOG_ERROR("Failed to remove existing FIFO.");
        return;
    }
    if (mkfifo(FIFONAME, 0660) < 0) {
        SPDLOG_ERROR("Failed to create FIFO. Error Code: {}, Error: {}", errno, strerror(errno));
        return;
    }
    mReadFifo = open(FIFONAME, O_RDONLY | O_NONBLOCK);
    if (mReadFifo < 0) {
        SPDLOG_ERROR("Failed to open read buffer. ErrorCode: {}, Error: {}", errno, strerror(errno));
        return;
    }
    mReadNotifier.setFd(mReadFifo);
    mReadNotifierConnection = mReadNotifier.notify.connect([this] {
        onGpsPositionReceived();
    });
    mProvider->start();
}

GpsdPositionInformationProvider::~GpsdPositionInformationProvider()
{
    mProvider->stop();
    close(mReadFifo);
}

GpsFixMode GpsdPositionInformationProvider::getGpsFixMode() const noexcept
{
    return mGpsFix;
}

std::uint8_t GpsdPositionInformationProvider::getNumbersOfStatelite() const
{
    return mSatellites;
}

void GpsdPositionInformationProvider::onGpsPositionReceived()
{
    auto buffer = Private::FifoData{};
    auto bytesRead = read(mReadFifo, &buffer, sizeof(buffer));
    if (bytesRead < 0) {
        SPDLOG_ERROR("Failed to read from FIFO. ErrorCode: {}, Error: {}", errno, strerror(errno));
    }
    auto ts = std::timespec{.tv_sec = buffer.timeSec, .tv_nsec = buffer.timeNanoSec};
    std::array<char, 16> timeBuff = {};
    std::strftime(&timeBuff[0], sizeof timeBuff, "%H:%M:%S", std::gmtime(&ts.tv_sec));
    size_t milliseconds = ts.tv_nsec / 1'000'000; // Convert ns to ms
    auto time = std::format("{}.{}", &timeBuff[0], milliseconds);
    std::array<char, 11> dateBuff = {};
    std::strftime(&dateBuff[0], sizeof(dateBuff), "%d.%m.%Y", std::gmtime(&ts.tv_sec));
    auto gpsPos = Common::GpsPositionData{Common::PositionData{buffer.latitude, buffer.longitude},
                                          {time},
                                          {std::string{&dateBuff[0]}},
                                          Common::VelocityData{buffer.velocity}};
    mSatellites = buffer.satellites;
    if (mGpsFix != buffer.gpsFix) {
        mGpsFix = buffer.gpsFix;
        gpsFixModeChanged.emit(mGpsFix);
    }
    gpsPosition.set(gpsPos);
}

} // namespace Rapid::Positioning
