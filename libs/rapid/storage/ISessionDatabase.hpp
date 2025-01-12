// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef ISESSIONDATABASE_HPP
#define ISESSIONDATABASE_HPP

#include "common/SessionData.hpp"
#include "system/AsyncResult.hpp"
#include <kdbindings/signal.h>
#include <memory>

namespace Rapid::Storage
{

/**
 * Alias for the @ref ISessionDatabase::getSessionByIndexAsync result.
 */
using GetSessionResult = System::AsyncResultWithValue<Common::SessionData>;

/**
 * Alias for the @ref ISessionDatabase::getSessionMetaDataByIndexAsync result.
 */
using GetSessionMetaDataResult = System::AsyncResultWithValue<Common::SessionMetaData>;

/**
 * The SessionDatabase provides an index based access to the stored session data.
 */
class ISessionDatabase
{
public:
    /**
     * Default destrutor
     */
    virtual ~ISessionDatabase() = default;

    /**
     * Deleted copy constructor
     */
    ISessionDatabase(ISessionDatabase const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    ISessionDatabase& operator=(ISessionDatabase const& other) = delete;

    /**
     * Deleted move constructor
     */
    ISessionDatabase(ISessionDatabase&& ohter) = delete;

    /**
     * Deleted move assignemnt operator
     */
    ISessionDatabase& operator=(ISessionDatabase&& other) = delete;

    /**
     * Gives the number of stored sessions and defines the width of indexs. The session
     * are accessable from 0...[SessionCount].
     * @return The number of stored sessions.
     */
    virtual std::size_t getSessionCount() = 0;

    /**
     * Gives the session by the index. If index doesn't exists a nullopt is returned.
     * @param index The index of the request session.
     * @return The session by the given index or a nullopt.
     */
    virtual std::optional<Common::SessionData> getSessionByIndex(std::size_t index) const noexcept = 0;

    /**
     * Gives the session by the index in async manner, so the call doesn't block the calling thread.
     * If index doesn't exists a the result is marked as error.
     * @param index The index of the request session.
     * @return The session by the given index or an error.
     */
    virtual std::shared_ptr<GetSessionResult> getSessionByIndexAsync(std::size_t index) noexcept = 0;

    /**
     * Gives the whole session defined by the session data in async manner, so the call doesn't block the calling thread.
     * If no session can be found for the passed meta data the result is marked as error.
     * @param metadata The metadata for which the session data shall be returned.
     * @return The session for the passed metadata or an error.
     */
    virtual std::shared_ptr<GetSessionResult> getSessionByMetadataAsync(
        Common::SessionMetaData const& metadata) noexcept = 0;

    /**
     * Gives the session meta data by the index in async manner, so the call doesn't block the calling thread.
     * If index doesn't exists a the result is marked as error.
     * @param index The index of the request session.
     * @return The session by the given index or an error.
     */
    virtual std::shared_ptr<GetSessionMetaDataResult> getSessionMetaDataByIndexAsync(std::size_t index) noexcept = 0;

    /**
     * Stores the given session.
     * @param session The session that shall bestored.
     * @return True session successful stored otherwise false.
     */
    virtual std::shared_ptr<System::AsyncResult> storeSession(Common::SessionData const& session) = 0;

    /**
     * Deletes the session under the given index.
     * If the index is not present nothing happens.
     * @param index The index which shall deleted.
     */
    virtual void deleteSession(std::size_t index) = 0;

    /**
     * This signal shall be emitted by a session database, when a new session is stored.
     * @param index The index that got added to the database.
     */
    KDBindings::Signal<std::size_t> sessionAdded;

    /**
     * This signal shall be emitted by a session database, when a session is updated.
     * @param index The index of the session that got updated.
     */
    KDBindings::Signal<std::size_t> sessionUpdated;

    /**
     * This signal shall be emitted by a session database when a session is deleted.
     * @param index The index of the session that got deleted.
     */
    KDBindings::Signal<std::size_t> sessionDeleted;

protected:
    ISessionDatabase() = default;
};

} // namespace Rapid::Storage

#endif // ISESSION_HPP
