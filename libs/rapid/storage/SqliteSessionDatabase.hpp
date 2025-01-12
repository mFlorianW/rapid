// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SQLITESESSIONDATABASE_HPP
#define SQLITESESSIONDATABASE_HPP

#include "ISessionDatabase.hpp"
#include "private/Connection.hpp"
#include "private/StorageContext.hpp"
#include <map>
#include <sqlite3.h>

namespace Rapid::Storage
{
class SqliteSessionDatabase : public ISessionDatabase
{
public:
    /**
     * Constructs a SqliteSessionDatabase
     * @param databaseFile the path to the SQLITE database.
     */
    explicit SqliteSessionDatabase(std::string const& databaseFile);

    /**
     * Destructor
     */
    ~SqliteSessionDatabase() override;

    /**
     * Deleted copy constructor
     */
    SqliteSessionDatabase(SqliteSessionDatabase const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    SqliteSessionDatabase& operator=(SqliteSessionDatabase const& other) = delete;

    /**
     * Deleted move constructor
     */
    SqliteSessionDatabase(SqliteSessionDatabase&& ohter) = delete;

    /**
     * Deleted move assignemnt operator
     */
    SqliteSessionDatabase& operator=(SqliteSessionDatabase&& other) = delete;

    /**
     * @copydoc ISessionDatabase::getSessionCount()
     */
    std::size_t getSessionCount() override;

    /**
     * @copydoc ISessionDatabase::getSessionByIndex(std::size_t index)
     */
    std::optional<Common::SessionData> getSessionByIndex(std::size_t index) const noexcept override;

    /**
     * @copydoc ISessionDatabase::getSessionByIndexAsync(std::size_t index)
     */
    std::shared_ptr<GetSessionResult> getSessionByIndexAsync(std::size_t index) noexcept override;

    /**
     * @copydoc ISessionDatabase::getSessionMetaDataByIndexAsync(Common::SessionMetaData const& metadata)
     */
    std::shared_ptr<GetSessionResult> getSessionByMetadataAsync(
        Common::SessionMetaData const& metadata) noexcept override;

    /**
     * @copydoc ISessionDatabase::getSessionMetaDataByIndexAsync(std::size_t index)
     */
    std::shared_ptr<GetSessionMetaDataResult> getSessionMetaDataByIndexAsync(std::size_t index) noexcept override;

    /**
     * @copydoc ISessionDatabase::storeSession(Common::SessionData &session)
     */
    std::shared_ptr<System::AsyncResult> storeSession(Common::SessionData const& session) override;

    /**
     * @copydoc ISessionDatabase::deleteSession(std::size_t index)
     */
    void deleteSession(std::size_t index) override;

private:
    void updateSession(Private::SessionStorageContext* ctx);
    void saveSession(Private::SessionStorageContext* ctx);
    void readSession(std::shared_ptr<Private::SessionStorageContextWithValue<Common::SessionData>> ctx) const;
    void readSessionMetaData(
        std::shared_ptr<Private::SessionStorageContextWithValue<Common::SessionMetaData>> ctx) const;
    void readSessionByMetaData(std::shared_ptr<Private::SessionStorageContextWithValue<Common::SessionData>> ctx) const;
    std::optional<std::size_t> readSessionIdOfIndex(std::size_t sessionIndex) const noexcept;
    std::optional<std::size_t> readSessionId(Common::SessionData const& session) const noexcept;
    std::optional<std::size_t> readIndexOfSessionId(std::size_t sessionId) const noexcept;
    std::vector<std::size_t> readSessionIds() const noexcept;
    std::optional<std::vector<Common::LapData>> readLapsOfSession(std::size_t sessionId) const noexcept;
    std::optional<Common::TrackData> readTrack(std::size_t trackId) const noexcept;
    bool saveLapOfSession(std::size_t sessionId, std::size_t lapIndex, Common::LapData const& lapData) const noexcept;
    bool saveLapLogPoints(std::size_t lapId, Common::LapData const& lapData) const noexcept;
    std::optional<std::size_t> readLapId(std::size_t sessionId, std::size_t lapIndex) const noexcept;
    std::optional<Common::SessionData> readSession(std::size_t index) const;
    std::optional<Common::SessionMetaData> readSessionMetaData(std::size_t index) const;
    std::optional<Common::SessionData> readSessionByMetaData(Common::SessionMetaData const& metadata) const;

    static void handleUpdates(void* objPtr, int event, char const* database, char const* table, sqlite3_int64 rowId);

    void updateIndexMapper();

    std::optional<std::size_t> getIndexForSessionId(std::size_t sessionDbId) const;

    std::shared_ptr<Private::Connection> mDbConnection;
    std::map<std::size_t, std::size_t> mIndexMapper;

    std::unordered_map<Private::StorageContextBase*, std::shared_ptr<Private::SessionStorageContext>> mStorageCache;
    std::mutex mutable mMutex;
};

} // namespace Rapid::Storage
#endif // SQLITESESSIONDATABASE_HPP
