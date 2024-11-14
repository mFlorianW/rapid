// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SQLITESESSIONDATABASE_HPP
#define SQLITESESSIONDATABASE_HPP

#include "FutureWatcher.hpp"
#include "ISessionDatabase.hpp"
#include "private/Connection.hpp"
#include <map>
#include <sqlite3.h>
#include <thread>

namespace Rapid::Storage
{
class AsyncResultDb;
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
     * @copydoc ISessionDatabase::storeSession(Common::SessionData &session)
     */
    std::shared_ptr<System::AsyncResult> storeSession(Common::SessionData const& session) override;

    /**
     * @copydoc ISessionDatabase::deleteSession(std::size_t index)
     */
    void deleteSession(std::size_t index) override;

private:
    struct StorageContext
    {
        StorageContext();
        ~StorageContext();

        StorageContext(StorageContext const& other) = delete;
        StorageContext& operator=(StorageContext const& ohter) = delete;

        StorageContext(StorageContext&& other) noexcept = delete;
        StorageContext& operator=(StorageContext&& ohter) = delete;

        std::size_t mSessionId{0};
        std::thread mStorageThread{};
        std::promise<bool> mStoragePromise;
        System::FutureWatcher<bool> mStorageResult;
        std::shared_ptr<AsyncResultDb> mResult;
        Common::SessionData mSession;

        KDBindings::Signal<StorageContext*> done;
    };

    void updateSession(StorageContext* ctx);
    void addSession(StorageContext* ctx);
    std::optional<std::size_t> getSessionIdOfIndex(std::size_t sessionIndex) const noexcept;
    std::optional<std::size_t> getSessionId(Common::SessionData const& session) const noexcept;
    std::optional<std::size_t> getIndexOfSessionId(std::size_t sessionId) const noexcept;
    std::vector<std::size_t> getSessionIds() const noexcept;
    std::optional<std::vector<Common::LapData>> getLapsOfSession(std::size_t sessionId) const noexcept;
    std::optional<Common::TrackData> getTrack(std::size_t trackId) const noexcept;
    bool storeLapOfSession(std::size_t sessionId, std::size_t lapIndex, Common::LapData const& lapData) const noexcept;
    bool storeLapLogPoints(std::size_t lapId, Common::LapData const& lapData) const noexcept;
    std::optional<std::size_t> getLapId(std::size_t sessionId, std::size_t lapIndex) const noexcept;

    static void handleUpdates(void* objPtr, int event, char const* database, char const* table, sqlite3_int64 rowId);

    void updateIndexMapper();

    Private::Connection& mDbConnection;
    std::map<std::size_t, std::size_t> mIndexMapper;

    std::unordered_map<StorageContext*, std::shared_ptr<StorageContext>> mStorageCache;
    std::mutex mutable mMutex;
};

} // namespace Rapid::Storage
#endif // SQLITESESSIONDATABASE_HPP
