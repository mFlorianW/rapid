// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef IACTIVESESSIONWORKFLOW_HPP
#define IACTIVESESSIONWORKFLOW_HPP

#include "common/SessionData.hpp"
#include <kdbindings/property.h>

namespace Rapid::Workflow
{
class IActiveSessionWorkflow
{
public:
    /**
     * Default virtual destructor.
     */
    virtual ~IActiveSessionWorkflow() = default;

    /**
     * Disabled copy operator
     */
    IActiveSessionWorkflow(IActiveSessionWorkflow const&) = delete;

    /**
     * Disabled copy operator
     */
    IActiveSessionWorkflow& operator=(IActiveSessionWorkflow const&) = delete;

    /**
     * Default move operator
     */
    IActiveSessionWorkflow(IActiveSessionWorkflow&&) noexcept = default;

    /**
     * Default move operator
     */
    IActiveSessionWorkflow& operator=(IActiveSessionWorkflow&&) noexcept = default;

    /**
     * Starts the ActiveSessionWorkflow.
     * Without calling start the active workflow session does nothing.
     */
    virtual void startActiveSession() noexcept = 0;

    /**
     * Stops the ActiveSessionWorkflow. No more laps are stored.
     */
    virtual void stopActiveSession() noexcept = 0;

    /**
     * Sets the track that shall be used for the active session.
     * @param track The new track for the session.
     */
    virtual void setTrack(Common::TrackData const& track) noexcept = 0;

    /**
     * @brief Gives the track that is used in the active session.
     *
     * @details The IActiveSessionWorkflow can only return a track when a track was set with @ref IActiveSessionWorkflow::setTrack.
     *
     * @return The track when set or a nullopt when no track is present.
     */
    virtual std::optional<Common::TrackData> getTrack() const noexcept = 0;

    /**
     * Gives the currently active session.
     * Only returns a valid session if startActiveSession is called.
     * @return The currently active session.
     */
    virtual std::optional<Common::SessionData> getSession() const noexcept = 0;

    /**
     * This signal is emitted when the ActiveWorkSessionFlow detects a finished lap.
     */
    KDBindings::Signal<> lapFinished;

    /**
     * This signal is emitted when the ActiveWorkSessionFlow detects a finished sector.
     */
    KDBindings::Signal<> sectorFinshed;

    /**
     * This property holds the current laptime.
     */
    KDBindings::Property<Common::Timestamp> currentLaptime;

    /**
     * This property holds the last laptime.
     */
    KDBindings::Property<Common::Timestamp> lastLaptime;

    /**
     * This property holds the current sector time.
     */
    KDBindings::Property<Common::Timestamp> currentSectorTime;

    /**
     * This property holds the last sector time.
     */
    KDBindings::Property<Common::Timestamp> lastSectorTime;

    /**
     * This property holds the lap count of the current session.
     */
    KDBindings::Property<std::size_t> lapCount;

protected:
    /**
     * Default protected constructor.
     */
    IActiveSessionWorkflow() = default;
};

} // namespace Rapid::Workflow

#endif // IACTIVESESSIONWORKFLOW_HPP
