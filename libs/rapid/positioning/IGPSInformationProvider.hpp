// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <kdbindings/signal.h>

namespace Rapid::Positioning
{

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
     * Gives the number of the connted satellites.
     * @return std::uint8_t
     */
    virtual std::uint8_t getNumbersOfStatelite() const = 0;

    /**
     * This signal is emitted when the number of statellites changes.
     * @return Signal<> The signal object to get notified when the number of statellite changes.
     */
    KDBindings::Signal<> numberOfSatellitesChanged;

protected:
    /**
     * Default protected constructor
     */
    IGpsInformationProvider() = default;
};

} // namespace Rapid::Positioning
