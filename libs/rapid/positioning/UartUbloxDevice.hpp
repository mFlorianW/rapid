// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_POSITIONING_UARTUBLOXDEVICE_HPP
#define RAPID_POSITIONING_UARTUBLOXDEVICE_HPP

#include <memory>
#include <positioning/IUbloxDevice.hpp>
#include <system/AsyncResult.hpp>

namespace Rapid::Positioning
{

/**
 * Forward declaration for receiving cc_ublox dispatch
 */
class UbloxDevicePrivate;

/**
 * UART based UBlox message provider for Linux.
 */
class UartUbloxDevice : public IUbloxDevice
{
public:
    /**
     * @brief Creates an instance of the @ref UartUbloxDevice
     *
     * @details There can only exists one @ref UartUbloxDataProvider because it's handled as exclusives resources
     *          Directly starts the initialization.
     *          This includes:
     *              - Disabling the NMEA2000 output
     *              - Switch to the from the default 9600 to 115200 baud rate.
     *          Afterwards the return value @ref IUbloxDevice::isReady is true and @ref IUbloxDevice::ready is emitted.
     *
     * @param uart The device node that shall be used. Something like "/dev/ttyUSB*" or "/dev/ttyACM*"
     */
    UartUbloxDevice(std::string uart);

    /**
     * Cleanup the resources.
     */
    ~UartUbloxDevice() override;

    /**
     * Disabled copy constructor
     */
    UartUbloxDevice(UartUbloxDevice const&) = delete;

    /**
     * Disabled copy operator
     */
    UartUbloxDevice& operator=(UartUbloxDevice const&) = delete;

    /**
     * Disabled move constructor
     */
    UartUbloxDevice(UartUbloxDevice&&) noexcept = default;

    /**
     * Default move operator
     */
    UartUbloxDevice& operator=(UartUbloxDevice&&) noexcept = default;

    /**
     * @copydoc IUbloxDevice::isReady
     */
    bool isReady() const noexcept override;

    /**
     * @copydoc IUbloxDevice::write
     */
    void write(std::vector<std::uint8_t> const& data) override;

    /**
     * @copydoc IUbloxDevice::read
     */
    std::vector<std::uint8_t> read() override;

    /**
     * @copydoc IUbloxDevice::hasDataPending
     */
    bool hasDataPending() const noexcept override;

private:
    friend class UbloxDevicePrivate;

    std::shared_ptr<System::AsyncResultWithValue<bool>> setupUart(std::uint32_t baudrate) noexcept;
    void configureUart() noexcept;
    void requestUartConfig() noexcept;
    void readRawMessage();

    std::unique_ptr<UbloxDevicePrivate> mD;
    KDBindings::ScopedConnection mFdNotifierConnection;
    KDBindings::ScopedConnection mInitializeTimeoutConnection;
};

} // namespace Rapid::Positioning

#endif // !RAPID_POSITIONING_UARTUBLOXDEVICE_HPP
