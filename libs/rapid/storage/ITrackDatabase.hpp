// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef ITRACKDATABASEBACKEND_HPP
#define ITRACKDATABASEBACKEND_HPP

#include <common/TrackData.hpp>
#include <kdbindings/signal.h>
#include <system/AsyncResult.hpp>

namespace Rapid::Storage
{
using AsyncTrackCountResult = System::AsyncResultWithValue<std::size_t>;
using AsyncTrackResult = System::AsyncResultWithValue<std::vector<Common::TrackData>>;

/**
 * The @ref ITrackDatabase defines the interface to store persistent @ref Common::TrackData.
 */
class ITrackDatabase
{
public:
    /**
     * Default destructor
     */
    virtual ~ITrackDatabase() = default;

    /**
     * Deleted copy constructor
     */
    ITrackDatabase(ITrackDatabase const& other) = delete;

    /**
     * Deleted copy assignment
     */
    ITrackDatabase& operator=(ITrackDatabase const& other) = delete;

    /**
     * Deleted move constructor
     */
    ITrackDatabase(ITrackDatabase&& other) = delete;

    /**
     * Deleted move assignment
     */
    ITrackDatabase& operator=(ITrackDatabase&& other) = delete;

    /**
     * Gives the number of stored tracks in the database.
     * The number from 0...[TrackCount-1] is the valid index range for track requests and deletions.
     * @return The nummber of stored tracks.
     */
    virtual std::size_t getTrackCount() = 0;

    /**
     * Gives the number of stored tracks in the database.
     * The number from 0...[TrackCount-1] is the valid index range for track requests and deletions.
     * @return The nummber of stored tracks.
     */
    virtual std::shared_ptr<AsyncTrackCountResult> getTrackCountAsync() = 0;

    /**
     * The loads all track data from the database.
     * @return The list with all stored tracks in the database on success, or nothing.
     */
    virtual std::vector<Common::TrackData> getTracks() = 0;

    /**
     * The loads all track data from the database.
     * Wait for the @AsyncResult::finished signal to be emitted before reading the result value.
     * @return The list with all stored tracks in the database on success, or nothing.
     */
    virtual std::shared_ptr<AsyncTrackResult> getTracksAsync() = 0;

    /**
     * Store the passed track in the database.
     * @param tracks The trackdata that shall be saved.
     * @return true Track data is successfull saved.
     * @return false The track data couldn't be saved.
     */
    virtual std::shared_ptr<System::AsyncResult> saveTrack(Common::TrackData const& track) = 0;

    /**
     * Deletes all track data.
     * @return true Track data successful deleted.
     * @return false Failed to delete the track data.
     */
    virtual std::shared_ptr<System::AsyncResult> deleteTrack(std::size_t trackIndex) = 0;

    /**
     * Deletes all track data.
     * @return true Track data successful deleted.
     * @return false Failed to delete the track data.
     */
    virtual std::shared_ptr<System::AsyncResult> deleteAllTracks() = 0;

    /**
     * The @ref ITrackDatabase::trackAdded is emitted when a track is added to the database.
     * @param index The index of the added track to the database.
     */
    KDBindings::Signal<std::size_t> trackAdded;

    /**
     * The @ref ITrackDatabase::trackDeleted is emitted when a track is deleted in the database.
     * If a track is deleted all user of the @ref SqliteTrackDatabase should drop the track under the index.
     * @param index The index of the deleted track database.
     */
    KDBindings::Signal<std::size_t> trackDeleted;

protected:
    /**
     * Default constructor
     */
    ITrackDatabase() = default;
};

} // namespace Rapid::Storage

#endif //! ITRACKDATABASEBACKEND_HPP
