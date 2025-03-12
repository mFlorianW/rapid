// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_POSITIONING_UBLOXGPSPOSITIONINFORMATIONPROVIDER_HPP
#define RAPID_POSITIONING_UBLOXGPSPOSITIONINFORMATIONPROVIDER_HPP

#include <positioning/IGPSInformationProvider.hpp>
#include <positioning/IGpsPositionProvider.hpp>
#include <positioning/IUbloxDevice.hpp>

namespace Rapid::Positioning
{
/**
 * Forward delcaration for the private implementation
 */
class UbloxGpsPositionInformationProviderPrivate;

/**
 * An Ublox based implementation for the @ref IGpsPositionProvider.
 * The configuration for receiving the necessary messages from the @ref IUbloxDevice happens automatically in the backgorund.
 * If an error occur during this process the signal @errorOccured is emitted.
 */
class UbloxGpsPositionInformationProvider final : public IGpsPositionProvider, public IGpsInformationProvider
{
public:
    /**
     * @brief Creates an instance of the @ref UbloxGpsPositionInformationProvider
     *
     * @details The constructor also start the configuration of the necessary NAV-PVT message that is needed to update the GPS position.
     *          In case of an error during this process the signal @ref errorOccured is emitted.
     *
     * @param device The device that shall be used to send and receive messages.
     */
    UbloxGpsPositionInformationProvider(std::unique_ptr<IUbloxDevice> device);

    /**
     * @brief Default destructor
     */
    ~UbloxGpsPositionInformationProvider() override;

    /**
     * @brief Disabled copy constructor
     */
    UbloxGpsPositionInformationProvider(UbloxGpsPositionInformationProvider const&) = delete;

    /**
     * @brief Disabled copy operator
     */
    UbloxGpsPositionInformationProvider& operator=(UbloxGpsPositionInformationProvider const&) = delete;

    /**
     * @brief Disabled move constructor
     */
    UbloxGpsPositionInformationProvider(UbloxGpsPositionInformationProvider&&) noexcept = delete;

    /**
     * @brief Disabled copy operator
     */
    UbloxGpsPositionInformationProvider& operator=(UbloxGpsPositionInformationProvider&&) noexcept = delete;

    /**
     * @copydoc Rapid::Positioning::IGPSInformationProvider::getGpsFixMode
     */
    GpsFixMode getGpsFixMode() const noexcept override;

    /**
     * @copydoc IGpsInformationProvider::getNumbersOfStatelite
     */
    std::uint8_t getNumbersOfStatelite() const override;

    /**
     * @brief This signal is emitted when @ref UbloxGpsPositionInformationProvider detects an error.
     * @tparam string A more detailed error message.
     */
    KDBindings::Signal<std::string> errorOccured;

private:
    std::unique_ptr<UbloxGpsPositionInformationProviderPrivate> mD;
};

} // namespace Rapid::Positioning

#endif // !RAPID_POSITIONING_UBLOXGPSPOSITIONINFORMATIONPROVIDER_HPP
