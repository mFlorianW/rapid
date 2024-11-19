// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SQLLITETRACKDATABASE_HPP
#define SQLLITETRACKDATABASE_HPP

#include "ITrackDatabase.hpp"
#include "private/Connection.hpp"
#include "private/StorageContext.hpp"
#include <LapData.hpp>
#include <optional>

namespace Rapid::Storage
{
class SqliteTrackDatabase : public ITrackDatabase
{
public:
    /**
     * Creates an Instance of the SqliteTrackDatabase
     * @param pathToDatabase The path to the Sqlite database file
     */
    SqliteTrackDatabase(std::string const& pathToDatabase);

    /**
     * Destructor
     */
    ~SqliteTrackDatabase() override;

    /**
     * Deleted copy constructor
     */
    SqliteTrackDatabase(SqliteTrackDatabase const& other) = delete;

    /**
     * Deleted copy assignment
     */
    SqliteTrackDatabase& operator=(SqliteTrackDatabase const& other) = delete;

    /**
     * Deleted move constructor
     */
    SqliteTrackDatabase(SqliteTrackDatabase&& other) = delete;

    /**
     * Deleted move assignment
     */
    SqliteTrackDatabase& operator=(SqliteTrackDatabase&& other) = delete;

    /**
     * @copydoc ITrackdatabase::getTrackCount()
     */
    std::size_t getTrackCount() override;

    /**
     * @copydoc ITrackdatabase::loadTracks()
     */
    std::vector<Common::TrackData> getTracks() override;

    /**
     * @copydoc ITrackdatabase::saveTrack(const std::vector<Common::TrackData> &tracks)
     */
    std::shared_ptr<System::AsyncResult> saveTrack(Common::TrackData const& tracks) override;

    /**
     * @copydoc ITrackdatabase::deleteTrack(std::size_t trackIndex)
     */
    std::shared_ptr<System::AsyncResult> deleteTrack(std::size_t trackIndex) override;

    /**
     * @copydoc ITrackdatabase::deleteAllTracks()
     */
    std::shared_ptr<System::AsyncResult> deleteAllTracks() override;

private:
    void deleteTrack(std::shared_ptr<Private::TrackStorageContext> ctx);
    void saveTrack(std::shared_ptr<Private::TrackStorageContext> ctx);
    void deleteAllTracks(std::shared_ptr<Private::TrackStorageContext> ctx);
    std::vector<std::size_t> getTrackIds() const noexcept;
    std::optional<std::size_t> getTrackIdOfIndex(std::size_t trackIndex) const noexcept;
    std::optional<std::size_t> savePosition(Common::PositionData const& position) const noexcept;
    std::optional<std::size_t> saveTrack(std::string const& name,
                                         std::size_t finishline,
                                         std::optional<std::size_t> startline) const noexcept;
    bool saveSection(std::size_t trackId, Common::PositionData const& section, std::size_t index);
    std::optional<std::size_t> getFinishlinePositionId(std::size_t trackId) const noexcept;
    std::optional<std::size_t> getStartlinePositionId(std::size_t trackId) const noexcept;
    bool deletePositionId(std::size_t positionId);
    std::vector<std::size_t> getSectionPositionIds(std::size_t trackId);

    Private::Connection& mDbConnection;
    std::unordered_map<Private::StorageContextBase*, std::shared_ptr<Private::TrackStorageContext>> mStorageCache;
    std::mutex mutable mMutex;
};

} // namespace Rapid::Storage

#endif // SQLLITETRACKDATABASE_HPP
