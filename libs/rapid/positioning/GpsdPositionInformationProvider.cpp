// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsdPositionInformationProvider.hpp"
#include "system/EventLoop.hpp"
#include <cmath>
#include <fcntl.h>
#include <filesystem>
#include <gps.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Rapid::Positioning
{

namespace Private
{

struct GpsdData
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
        if (mRunning) {
            gps_stream(&mGpsd, WATCH_DISABLE, nullptr);
            stop();
            if (mGpsProvider.joinable()) {
                mGpsProvider.join();
            }
            gps_close(&mGpsd);
        }
    }

    GpsdProvider(GpsdProvider const&) = delete;
    GpsdProvider& operator=(GpsdProvider const&) = delete;
    GpsdProvider(GpsdProvider&&) noexcept = delete;
    GpsdProvider& operator=(GpsdProvider&&) noexcept = delete;

    void start()
    {
        if (not mRunning) {
            mGpsProvider = std::thread{&GpsdProvider::run, this};
            mRunning = true;
        }
    }

    void stop()
    {
        mRunning = false;
    }

    Common::GpsPositionData getPosition() const noexcept
    {
        auto guard = std::lock_guard(mAccessMutex);
        return mPosition;
    }

    GpsFixMode getGpsFixMode() const noexcept
    {
        auto guard = std::lock_guard(mAccessMutex);
        return mGpsFix;
    }

    std::size_t getStatellites() const noexcept
    {
        auto guard = std::lock_guard(mAccessMutex);
        return mSatellites;
    }

    KDBindings::Signal<> changed;

private:
    void run()
    {
        auto error = gps_open("localhost", "2947", &mGpsd);
        if (error < 0) {
            SPDLOG_ERROR("Failed to connected GPSD. Error: {}", gps_errstr(error));
            return;
        }
        gps_stream(&mGpsd, WATCH_ENABLE | WATCH_JSON, nullptr);
        auto buffer = Private::GpsdData{};
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

            auto ts = std::timespec{.tv_sec = buffer.timeSec, .tv_nsec = buffer.timeNanoSec};
            std::array<char, 16> timeBuff = {};
            std::strftime(&timeBuff[0], sizeof timeBuff, "%H:%M:%S", std::gmtime(&ts.tv_sec));
            size_t milliseconds = ts.tv_nsec / 1'000'000; // Convert ns to ms
            auto time = std::format("{}.{}", &timeBuff[0], milliseconds);
            std::array<char, 11> dateBuff = {};
            std::strftime(&dateBuff[0], sizeof(dateBuff), "%d.%m.%Y", std::gmtime(&ts.tv_sec));

            {
                auto guard = std::lock_guard(mAccessMutex);
                mPosition = Common::GpsPositionData{Common::PositionData{buffer.latitude, buffer.longitude},
                                                    {time},
                                                    {std::string{&dateBuff[0]}},
                                                    Common::VelocityData{buffer.velocity}};
                mSatellites = buffer.satellites;
                mGpsFix = buffer.gpsFix;
            }
            changed.emit();
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
    mutable std::mutex mAccessMutex;
    Common::GpsPositionData mPosition;
    GpsFixMode mGpsFix{GpsFixMode::NoFix};
    std::size_t mSatellites{0};
};

} // namespace Private

std::unique_ptr<Private::GpsdProvider> GpsdPositionInformationProvider::sProvider =
    std::make_unique<Private::GpsdProvider>();

GpsdPositionInformationProvider::GpsdPositionInformationProvider()
{
    mChangedConnection = sProvider->changed.connectDeferred(System::EventLoop::getConnectionEvaluator(), [this] {
        auto const gpsPos = sProvider->getPosition();
        auto newPos =
            Common::GpsPositionData{gpsPos.getPosition(), gpsPos.getTime(), gpsPos.getDate(), gpsPos.getVelocity()};
        gpsPosition.set(newPos);
        auto const satellites = sProvider->getStatellites();
        if (mSatellites != satellites) {
            mSatellites = satellites;
            numberOfSatellitesChanged.emit();
        }
        auto const gpsFix = sProvider->getGpsFixMode();
        if (mGpsFix != gpsFix) {
            mGpsFix = gpsFix;
            gpsFixModeChanged.emit(mGpsFix);
        }
    });
    sProvider->start();
}
GpsdPositionInformationProvider::~GpsdPositionInformationProvider() = default;

GpsFixMode GpsdPositionInformationProvider::getGpsFixMode() const noexcept
{
    return mGpsFix;
}

std::uint8_t GpsdPositionInformationProvider::getNumbersOfStatelite() const
{
    return mSatellites;
}

} // namespace Rapid::Positioning
