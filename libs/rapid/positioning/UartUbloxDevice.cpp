// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "positioning/UartUbloxDevice.hpp"
#include "system/Timer.hpp"
#include <cc_ublox/Message.h>
#include <cc_ublox/frame/UbloxFrame.h>
#include <cc_ublox/message/CfgPrtPortPoll.h>
#include <cc_ublox/message/CfgPrtUart.h>
#include <comms/process.h>
#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <system/FdNotifier.hpp>
#include <termios.h>
#include <unistd.h>

using namespace std::chrono_literals;

namespace Rapid::Positioning
{

namespace
{

std::string baudRateToString(std::uint32_t baudRate)
{
    switch (baudRate) {
    case B115200:
        return "115200";
    default:
        return {"Unknown"};
    }
    return {};
}

} // namespace

class UbloxDevicePrivate
{
public:
    using InMessage =
        cc_ublox::Message<comms::option::ReadIterator<std::uint8_t*>, comms::option::Handler<UbloxDevicePrivate>>;

    using Outbuffer = std::vector<std::uint8_t>;
    using OutMessage = cc_ublox::Message<comms::option::IdInfoInterface,
                                         comms::option::WriteIterator<std::back_insert_iterator<Outbuffer>>,
                                         comms::option::LengthInfoInterface>;

    using InAckMsg = cc_ublox::message::AckAck<InMessage>;
    using InNakMsg = cc_ublox::message::AckNak<InMessage>;
    using InCfgPrtUart = cc_ublox::message::CfgPrtUart<InMessage>;

    using AllInMessages = std::tuple<InNakMsg, InAckMsg, InCfgPrtUart>;
    using UbloxFrame = cc_ublox::frame::UbloxFrame<InMessage, AllInMessages>;

public:
    UbloxDevicePrivate(UartUbloxDevice& receiver)
        : mReceiver{receiver}
    {
    }

    void handle(InMessage const& msg)
    {
        SPDLOG_WARN("Not requested message received");
    }

    void handle(InCfgPrtUart const& portCfg)
    {
        SPDLOG_INFO("CFG port response message received.");
        portConfiguration = portCfg;
        mReceiver.configureUart();
    }

    void handle(InAckMsg const& ack)
    {
        auto msgId = ack.field_msgId().getValue();
        auto msgClass = (msgId >> 8) & 0xFF;
        auto id = msgId & 0xFF;
        SPDLOG_INFO("Ack message received class: {:#x}, id: {:#x}", msgClass, id);
        if (msgId == cc_ublox::MsgId::MsgId_CfgPrt and not portConfigured) {
            SPDLOG_INFO("Successfull send UBX-CFG-PRT poll message.");
        } else if (msgId == cc_ublox::MsgId::MsgId_CfgPrt and portConfigured) {
            SPDLOG_INFO("Successfull send UBX-CFG-PRT configuration message.");
            SPDLOG_INFO("UBlox device successful initialized.");
            initialized = true;
            initializeTimer.stop();
            mReceiver.ready.emit();
        }
    }

    void handle(InNakMsg const& nack)
    {
        auto msgId = static_cast<std::uint16_t>(nack.field_msgId().getValue());
        auto msgClass = (msgId >> 8) & 0xFF;
        auto id = msgId & 0xFF;
        SPDLOG_WARN("Nack message received class: {:#x},  id: {:#x}", msgClass, id);
    }

    void processData() noexcept
    {
        if (!msgCache.empty()) {
            auto processed = comms::processAllWithDispatch(msgCache.data(), msgCache.size(), mFrame, *this);
            msgCache.erase(msgCache.begin(), msgCache.begin() + static_cast<long>(processed));
        }
    }

    std::vector<std::uint8_t> serializeMessage(OutMessage const& message)
    {
        auto buffer = Outbuffer{};
        buffer.reserve(message.length());
        auto iter = std::back_insert_iterator(buffer);
        auto es = mFrame.write(message, iter, buffer.max_size());
        if (es == comms::ErrorStatus::UpdateRequired) {
            auto updateIter = &buffer[0];
            es = mFrame.update(updateIter, buffer.size());
        }

        if (es != comms::ErrorStatus::Success) {
            SPDLOG_ERROR("Failed to send ublox message {} with error: {}",
                         static_cast<int>(message.getId()),
                         static_cast<int>(es));
            return {};
        }
        return buffer;
    }

    std::array<std::uint8_t, 512> inBuffer{0};
    std::vector<std::uint8_t> msgCache;
    InCfgPrtUart portConfiguration;
    std::string device;
    bool initialized{false};
    int uartFd{-1};
    std::unique_ptr<System::FdNotifier> readNotifier;
    System::Timer initializeTimer;
    System::Timer configurationTimer;
    bool portConfigured{false};
    KDBindings::ScopedConnection mFdNotifierConnection;
    KDBindings::ScopedConnection mInitializeTimeoutConnection;
    KDBindings::ScopedConnection configurationTimerConnection;
    std::shared_ptr<System::AsyncResultWithValue<bool>> configurationResult;
    KDBindings::ScopedConnection configurationResultConnection;

private:
    UartUbloxDevice& mReceiver;
    UbloxFrame mFrame;
};

UartUbloxDevice::UartUbloxDevice(std::string uart)
    : mD{std::make_unique<UbloxDevicePrivate>(*this)}
{
    mD->device = uart;
    mD->uartFd = open(mD->device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);
    if (mD->uartFd < 0) {
        SPDLOG_ERROR("Failed to open UBlox device message provider will not work. Error: {}", strerror(errno));
        return;
    }

    mD->readNotifier = std::make_unique<System::FdNotifier>(mD->uartFd, System::FdNotifierType::Read);
    mD->mFdNotifierConnection = mD->readNotifier->notify.connect(&UartUbloxDevice::readRawMessage, this);

    if (flock(mD->uartFd, LOCK_EX) < 0) {
        SPDLOG_ERROR("Failed to set exclusive lock on the UART {}. Error: {}", mD->device, strerror(errno));
    }

    mD->initializeTimer.setInterval(std::chrono::milliseconds{3000});
    mD->mInitializeTimeoutConnection = mD->initializeTimer.timeout.connect([this] {
        SPDLOG_ERROR("UBlox device not initialized after {} seconds. Restarting initialization",
                     mD->initializeTimer.getInterval().count() / 1000);
        requestUartConfig();
    });
    mD->configurationResult = setupUart(B115200);
    if (mD->configurationResult->getResult() == System::Result::Error) {
        return;
    }
    mD->configurationResultConnection = mD->configurationResult->done.connect([this](auto*) {
        mD->initializeTimer.start();
        requestUartConfig();
    });
}

UartUbloxDevice::~UartUbloxDevice()
{
    // explicit set the notifier to nullpointer to correctly unregister the events before closing the fd.
    mD->readNotifier = nullptr;
    if (mD->uartFd > 0) {
        close(mD->uartFd);
    }
};

bool UartUbloxDevice::isReady() const noexcept
{
    return false;
}

void UartUbloxDevice::write(std::vector<std::uint8_t> const& data)
{
    auto bytesWritten = ::write(mD->uartFd, data.data(), data.size());
    if (bytesWritten != static_cast<ssize_t>(data.size())) {
        SPDLOG_WARN("Failed to send all bytes. Only send {} of {} bytes", bytesWritten, data.size());
    }
}

std::vector<std::uint8_t> UartUbloxDevice::read()
{
    if (mD->initialized) {
        auto result = std::vector<std::uint8_t>{};
        result.swap(mD->msgCache);
        return result;
    }
    return {};
}

bool UartUbloxDevice::hasDataPending() const noexcept
{
    if (mD->initialized) {
        return mD->msgCache.size() > 0;
    }
    return false;
}

std::shared_ptr<System::AsyncResultWithValue<bool>> UartUbloxDevice::setupUart(std::uint32_t baudrate) noexcept
{
    auto result = std::make_shared<System::AsyncResultWithValue<bool>>();
    auto options = termios{};
    if (tcgetattr(mD->uartFd, &options) < 0) {
        SPDLOG_ERROR("Failed to read UBlox UART options. Error: {}", strerror(errno));
        result->setResult(System::Result::Error);
        result->setResultValue(false);
        return result;
    }
    cfmakeraw(&options);

    // // Configure 8N1 (8 data bits, no parity, 1 stop bit)
    // options.c_cflag = (options.c_cflag & ~CSIZE) | CS8; // 8-bit characters
    // options.c_cflag &= ~PARENB; // No parity bit
    // options.c_cflag &= ~CSTOPB; // 1 stop bit
    // options.c_cflag &= ~CRTSCTS; // Disable hardware flow control (RTS/CTS)
    //
    // // Enable the receiver and set local mode
    // options.c_cflag |= (CLOCAL | CREAD);
    //
    // // Disable canonical mode, echo, and signal characters
    // options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //
    // // Disable software flow control
    // options.c_iflag &= ~(IXON | IXOFF | IXANY);
    //
    // // Raw output mode
    // options.c_oflag &= ~OPOST;

    if (cfsetispeed(&options, baudrate) < 0) {
        SPDLOG_ERROR("Failed ot set in speed {} for UBlox UART. Error: {}",
                     baudRateToString(baudrate),
                     strerror(errno));
        result->setResult(System::Result::Error);
        result->setResultValue(false);
        return result;
    }

    if (cfsetospeed(&options, baudrate) < 0) {
        SPDLOG_ERROR("Failed ot set out speed {} for UBlox UART. Error: {}",
                     baudRateToString(baudrate),
                     strerror(errno));
        result->setResult(System::Result::Error);
        result->setResultValue(false);
        return result;
    }

    // directly switching the baudrate causes problems on the PI3.
    // Even with TCSAFLUSH or TCSADRAIN the data are not correctly send.
    // An extra delay of 50ms makes sure that the data is correctly send.
    mD->configurationTimerConnection = mD->configurationTimer.timeout.connect([this, options, result] {
        if (tcsetattr(mD->uartFd, TCSANOW, &options) < 0) {
            SPDLOG_ERROR("Failed to set UBlox UART options. Error: {}", strerror(errno));
            return;
        }
        mD->configurationTimer.stop();
        result->setResult(System::Result::Ok);
        result->setResultValue(false);
    });
    mD->configurationTimer.setInterval(std::chrono::milliseconds{250});
    mD->configurationTimer.start();
    return result;
}

void UartUbloxDevice::configureUart() noexcept
{
    using OutCfgPrtUart = cc_ublox::message::CfgPrtUart<UbloxDevicePrivate::OutMessage>;
    OutCfgPrtUart msg;

    auto& outProtoMaskField = msg.field_outProtoMask();
    using OutProtoMaskField = typename std::decay<decltype(outProtoMaskField)>::type;
    outProtoMaskField.setBitValue(OutProtoMaskField::BitIdx_outNmea, false);
    outProtoMaskField.setBitValue(OutProtoMaskField::BitIdx_outUbx, true);

    msg.field_portId().setValue(mD->portConfiguration.field_portId().value());
    auto& inProtoMaskField = msg.field_inProtoMask();
    using InProtoMaskField = typename std::decay<decltype(inProtoMaskField)>::type;
    inProtoMaskField.setBitValue(InProtoMaskField::BitIdx_inNmea, true);
    inProtoMaskField.setBitValue(InProtoMaskField::BitIdx_inUbx, true);

    msg.field_baudRate().setValue(115200);
    msg.field_txReady().setValue(mD->portConfiguration.field_txReady().getValue());
    msg.field_mode().setValue(mD->portConfiguration.field_mode().getValue());
    msg.field_cfgPrtFlags().setValue(mD->portConfiguration.field_cfgPrtFlags().getValue());

    auto rawMsg = mD->serializeMessage(msg);
    write(rawMsg);
    mD->portConfigured = true;
    SPDLOG_INFO("Send UART configuration for {} baud with NMEA disabled and UBX enabled.", baudRateToString(B115200));
}

void UartUbloxDevice::readRawMessage()
{
    auto bytesRead = ::read(mD->uartFd, mD->inBuffer.data(), mD->inBuffer.size());
    if (bytesRead < 0) {
        SPDLOG_ERROR("Failed to read data from UBLOX UART. Error Code: {} Error: {}", errno, strerror(errno));
        return;
    }
    mD->msgCache.insert(mD->msgCache.end(), mD->inBuffer.cbegin(), mD->inBuffer.cbegin() + bytesRead);
    if (not mD->initialized) {
        mD->processData();
    } else if (bytesRead > 0) {
        dataReady.emit();
    }
}

void UartUbloxDevice::requestUartConfig() noexcept
{
    auto msg = cc_ublox::message::CfgPrtPortPoll<UbloxDevicePrivate::OutMessage>();
    msg.field_portId().setValue(std::uint8_t{1});
    auto rawMsg = mD->serializeMessage(msg);
    write(rawMsg);
    SPDLOG_INFO("Request UART configuration for initialization.");
}

} // namespace Rapid::Positioning
