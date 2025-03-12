// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <kdbindings/property.h>
#include <kdbindings/signal.h>

namespace Rapid::Positioning
{

/**
 * The fix modes that a GPS device can have.
 */
enum class GpsFixMode
{
    /** This is the normally the state when the GPS device is started */
    NoFix,
    /** The GPS system has a fix status that is good for latitude and longitude */
    Fix2d,
    /** The GPS system has a fix status that is good for latitude, longitude and altitude/climb */
    Fix3d,
};

/**
 * Interface for providing GPS information
 */
class IGpsInformationProvider
{
public:
    /**
     * Default destructor
     */
    virtual ~IGpsInformationProvider() = default;

    /**
     * Disabled copy constructor
     */
    IGpsInformationProvider(IGpsInformationProvider const&) = delete;

    /**
     * Disable copy operator
     */
    IGpsInformationProvider& operator=(IGpsInformationProvider const&) = delete;

    /**
     * Default move operator
     */
    IGpsInformationProvider(IGpsInformationProvider&&) noexcept = default;

    /**
     * Default move operator
     */
    IGpsInformationProvider& operator=(IGpsInformationProvider&&) noexcept = default;

    /**
     * @brief Gives the currently detected fix mode of the GPS device.
     * @return The fix mode of the GPS system.
     */
    virtual GpsFixMode getGpsFixMode() const noexcept = 0;

    /**
     * Gives the number of the connted satellites.
     * @return std::uint8_t
     */
    virtual std::uint8_t getNumbersOfStatelite() const = 0;

    /**
     * This signal is emitted when the number of statellites changes.
     * @return Signal<> The signal object to get notified when the number of statellite changes.
     */
    KDBindings::Signal<> numberOfSatellitesChanged;

    /**
     * This signal is emitted when a GPS device changes it's fix mode.
     * @tparam  GpsFixMode The fix mode of the GPS device.
     */
    KDBindings::Signal<GpsFixMode> gpsFixModeChanged;

protected:
    /**
     * Default protected constructor
     */
    IGpsInformationProvider() = default;
};

} // namespace Rapid::Positioning
