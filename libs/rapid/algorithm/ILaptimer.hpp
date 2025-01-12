// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <common/GpsPositionData.hpp>
#include <common/Timestamp.hpp>
#include <common/TrackData.hpp>
#include <kdbindings/property.h>

namespace Rapid::Algorithm
{

class ILaptimer
{
public:
    /**
     * Default desctructor.
     */
    virtual ~ILaptimer() = default;

    /**
     * Disabled copy operator
     */
    ILaptimer(ILaptimer const&) = delete;

    /**
     * Disabled copy operator
     */
    ILaptimer& operator=(ILaptimer const&) = delete;

    /**
     * Default move operator
     */
    ILaptimer(ILaptimer&&) noexcept = default;

    /**
     * Default move operator
     */
    ILaptimer& operator=(ILaptimer&&) noexcept = default;

    /**
     * Sets the Track in the laptimer.
     * The track information is needed to calculate the section and laptime.
     * @param track The new track that is used by the laptimer.
     */
    virtual void setTrack(Common::TrackData const& track) = 0;

    /**
     * Update the position in the laptimer.
     * The Position is used in the laptimer to calculate the section and laptime.
     * @param data The new position.
     */
    virtual void updatePositionAndTime(Common::GpsPositionData const& data) = 0;

    /**
     * @return Common::Timestamp Gives the last laptime.
     */
    virtual Common::Timestamp getLastLaptime() const = 0;

    /**
     * @return Common::Timestamp Gives the last sector time.
     */
    virtual Common::Timestamp getLastSectorTime() const = 0;

    /**
     * This signal is emitted when a lap is started.
     */
    KDBindings::Signal<> lapStarted;

    /**
     * This signal is emitted when a lap is finished.
     */
    KDBindings::Signal<> lapFinished;

    /**
     * This signal is emitted when a sector is finished.
     */
    KDBindings::Signal<> sectorFinished;

    /**
     * This property holds the current active laptime.
     */
    KDBindings::Property<Common::Timestamp> currentLaptime;

    /**
     * This property holds the current active sectiom time.
     */
    KDBindings::Property<Common::Timestamp> currentSectorTime;

protected:
    /**
     * Default constructor
     */
    ILaptimer() = default;
};

} // namespace Rapid::Algorithm
