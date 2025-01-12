// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_POSITIONING_IUBLOXMESSAGEPROVIDER_HPP
#define RAPID_POSITIONING_IUBLOXMESSAGEPROVIDER_HPP

#include <cstdint>
#include <kdbindings/signal.h>
#include <vector>

namespace Rapid::Positioning
{

/**
 * @brief The @ref IUbloxDevice defines the interface for receiving raw UBX protocol data.
 *
 * @details An @ref IUbloxDevice brings up the Ublox hardware:
 *          As an exmpale for an UART Ublox device this includes:
 *          - Disabling the NEMA2000
 *          - Enabling the UBX protocol output
 *          - Setup the baud rate from 9600 to 11500
 *          All other operations are not in the scope of an Ublox
 */
class IUbloxDevice
{
public:
    /**
     * Default destructor
     */
    virtual ~IUbloxDevice() = default;

    /**
     * Disabled copy constructor
     */
    IUbloxDevice(IUbloxDevice const&) = delete;

    /**
     * Disabled copy operator
     */
    IUbloxDevice& operator=(IUbloxDevice const&) = delete;

    /**
     * Default move constructor
     */
    IUbloxDevice(IUbloxDevice&&) noexcept = default;

    /**
     * Default move constructor
     */
    IUbloxDevice& operator=(IUbloxDevice&&) noexcept = default;

    /**
     * @brief Provider is ready for sending and receiving
     *
     * @details If the provider is not ready, returns false.
     *          The ready status is reported with @ref IUbloxMessageProvider::ready signal.
     *
     * @return Ready for sending and receiving
     */
    virtual bool isReady() const noexcept = 0;

    /**
     * @brief Sends the data to the Ublox receiver.
     *
     * @details The result of the write is notified with the @ref IUbloxMessageProvider::dataReady signal.
     *          The result is typically reported by a ACK/NACK message.
     *
     * @param data The data that shall be send to the Ublox receiver
     */
    virtual void write(std::vector<std::uint8_t> const& data) = 0;

    /**
     * @brief Gives the next received data.
     *
     * @details This function only returns valid data when the signal @ref IUbloxMessageProvider::dataReady is emitted.
     *          Use the @ref IUbloxMessageProvider::hasDataPending to check for further data.
     *
     * @return The next received data.
     */
    virtual std::vector<std::uint8_t> read() = 0;

    /**
     * @brief Checks for messages for processing.
     *
     * @details The functions returns true when there are data for processing, otherwise false.
     *          This functions doesn't need to be polled.
     *          When messages for processing are received the signal @ref IUbloxMessageProvider::dataReady is emitted.
     *
     * @return True data are pending for processing otherwise false
     */
    virtual bool hasDataPending() const noexcept = 0;

    /**
     * @brief Signal to indicate ready for use.
     *
     * @details This signal is emitted when the provider is ready for use and is only emitted once.
     *          So before connecting to signal the state should be checked with @ref IUbloxMessageProvider::isReady.
     *          A connect to this signal should only be happen when @ref IUbloxMessageProvider::isReady returns false.
     */
    KDBindings::Signal<> ready;

    /**
     * @brief Signal to indicate for data to process
     *
     * @details This signal is only emitted once when the receive message buffer is empty and the provides is ready for use.
     *          So when handling this signal, all pending messages should be read until the buffer is empty.
     */
    KDBindings::Signal<> dataReady;

protected:
    IUbloxDevice() = default;
};

} // namespace Rapid::Positioning

#endif // RAPID_POSITIONING_IUBLOXMESSAGEPROVIDER_HPP
