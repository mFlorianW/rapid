// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPD_WORKFLOW_IRESTACTIVESESSION_HPP
#define RAPD_WORKFLOW_IRESTACTIVESESSION_HPP

#include <common/Timestamp.hpp>
#include <common/TrackData.hpp>
#include <kdbindings/property.h>
#include <kdbindings/signal.h>

namespace Rapid::Workflow
{

/**
 * Defines the interface for the Active Session where the data is provided by the @ref ActiveSessionEndpoint
 */
class IRestActiveSession
{
public:
    /**
     * Default destructor
     */
    ~IRestActiveSession() = default;

    /**
     * Disabled copy constructor
     */
    IRestActiveSession(IRestActiveSession const&) = delete;

    /**
     * Disabled copy operator
     */
    IRestActiveSession& operator=(IRestActiveSession const&) = delete;

    /**
     * Disabled move constructor
     */
    IRestActiveSession(IRestActiveSession&&) noexcept = delete;

    /**
     * Disabled move operator
     */
    IRestActiveSession& operator=(IRestActiveSession&&) noexcept = delete;

    /**
     * @brief Update the tracks property
     *
     * @details The result is notified by the change the signal of the @ref Rapid::Workflow::IRestActiveSession::track property.
     */
    virtual void updateTrackData() noexcept = 0;

    /**
     * @brief Update the lap properties
     *
     * @details The result is notified by the change the signal of the lap properties.
     */
    virtual void updateLapData() noexcept = 0;

    /**
     * @brief Gives the track of the active session
     *
     * @details The property can be updated by calling the @ref IRestActiveSession::updateTrackData
     */
    KDBindings::Property<Common::TrackData> track;

    /**
     * @brief Gives the lap count of the active session
     *
     * @details The property can be updated by calling the @ref IRestActiveSession::updateLapData
     */
    KDBindings::Property<std::size_t> lapCount;

    /**
     * @brief Gives the current lap time of the active session
     *
     * @details The property can be updated by calling the @ref IRestActiveSession::updateLapData
     */
    KDBindings::Property<Common::Timestamp> currentLap;

    /**
     * @brief Gives the current sector time of the active session
     *
     * @details The property can be updated by calling the @ref IRestActiveSession::updateLapData
     *          This property has only a value when the track defines sectors.
     */
    KDBindings::Property<Common::Timestamp> currentSector;

    /**
     * @brief Gives the last lap time of the active session
     *
     * @details The property can be updated by calling the @ref IRestActiveSession::updateLapData
     */
    KDBindings::Property<Common::Timestamp> lastLap;

    /**
     * @brief Gives the lap last sector of the active session
     *
     * @details The property can be updated by calling the @ref IRestActiveSession::updateLapData
     *          This property has only a value when the track defines sectors.
     */
    KDBindings::Property<Common::Timestamp> lastSector;

protected:
    IRestActiveSession() = default;
};

} // namespace Rapid::Workflow

#endif // !RAPD_WORKFLOW_IRESTACTIVESESSION_HPP
