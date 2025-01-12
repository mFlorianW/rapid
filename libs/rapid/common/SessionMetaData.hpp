// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Date.hpp"
#include "Timestamp.hpp"
#include "TrackData.hpp"

namespace Rapid::Common
{

class SharedSessionMetaData;
/**
 * Holds the meta data of a session without the laps and all the log points.
 * This class makes it possible to cache session data with less memory than with the @ref SessionData.
 */
class SessionMetaData
{
public:
    /**
     * Creates an empty SessionMetaData instance
     */
    SessionMetaData();

    /**
     * Creates a SessionMetaData instance
     * @param track The track for the session
     * @param date The date of the session
     * @param time The time of the session
     * @param id The ID of the session for identifying the data session in a repository
     */
    SessionMetaData(TrackData track, Date date, Timestamp time, std::size_t id = 0);

    /**
     * Default destructor
     */
    ~SessionMetaData();

    /**
     * Copy constructor
     */
    SessionMetaData(SessionMetaData const&);

    /**
     * Copy assignment operator
     */
    SessionMetaData& operator=(SessionMetaData const&);

    /**
     * Move constructor
     */
    SessionMetaData(SessionMetaData&&) noexcept;

    /**
     * Move assignment operator
     */
    SessionMetaData& operator=(SessionMetaData&&) noexcept;

    /**
     * @brief Gets the session ID for identifying the data in a repository
     *
     * @details
     * The session id is set once on creation.
     * The value of the sessiond ID depends on who creates the session data.
     * E.g. for a database the session ID will have the meaning to identify the session in the database.
     *
     * @return The session ID for identifying the session in a repository.
     */
    std::size_t getId() const noexcept;

    /**
     * Gives the date of the session.
     * @return  The date of the session.
     */
    Date getSessionDate() const noexcept;

    /**
     * Gives the time of the session.
     * @return The date of the session.
     */
    Timestamp getSessionTime() const noexcept;

    /**
     * Gives the track of the session.
     * @return The track of the session.
     */
    TrackData const& getTrack() const noexcept;

    /**
     * Default compare operator
     * @param rhs The SessionMetaData to compare with.
     */
    bool operator==(SessionMetaData const& rhs) const noexcept;

    /**
     * Default compare operator
     * @param rhs The SessionMetaData to compare with.
     */
    bool operator!=(SessionMetaData const& rhs) const noexcept;

private:
    SharedDataPointer<SharedSessionMetaData> mData;
};

} // namespace Rapid::Common
