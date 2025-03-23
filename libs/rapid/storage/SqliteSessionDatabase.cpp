// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SqliteSessionDatabase.hpp"
#include "private/Statement.hpp"
#include <algorithm>
#include <cstring>
#include <spdlog/spdlog.h>

using namespace Rapid::Storage::Private;

namespace Rapid::Storage
{

SqliteSessionDatabase::SqliteSessionDatabase(std::string const& databaseFile)
    : mDbConnection{Connection::connection(databaseFile)}
{
    auto* rawHandle = mDbConnection->getRawHandle();
    sqlite3_update_hook(rawHandle, &SqliteSessionDatabase::handleUpdates, this);
    updateIndexMapper();
}

SqliteSessionDatabase::~SqliteSessionDatabase()
{
    auto* rawHandle = mDbConnection->getRawHandle();
    sqlite3_update_hook(rawHandle, nullptr, nullptr);
    for (auto& [result, context] : mStorageCache) {
        if (context->mStorageThread.joinable()) {
            context->mStorageThread.join();
        }
    }
}

std::size_t SqliteSessionDatabase::getSessionCount()
{
    return mIndexMapper.size();
}

std::optional<Common::SessionData> SqliteSessionDatabase::getSessionByIndex(std::size_t index) const noexcept
{
    std::lock_guard<std::mutex> const guard{mMutex};
    return readSession(index);
}

std::shared_ptr<GetSessionResult> SqliteSessionDatabase::getSessionByIndexAsync(std::size_t index) noexcept
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto result = std::make_shared<GetSessionResult>();
    auto context = std::make_shared<Private::SessionStorageContextWithValue<Common::SessionData>>(result);
    mStorageCache.emplace(context.get(), context);
    context->mSessionId = index;
    std::ignore = context->done.connect([this](Private::StorageContextBase* ctx) {
        auto sessionCtx =
            StorageContextBase::getStorageAs<SessionStorageContextWithValue<Common::SessionData>>(mStorageCache[ctx]);
        auto const result = sessionCtx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        if (result == System::Result::Ok) {
            auto const getResult = sessionCtx->getResultAs<GetSessionResult>();
            getResult->setResultValue(sessionCtx->value);
        }
        sessionCtx->mResult->setResult(result);
        if (sessionCtx->mStorageThread.joinable()) {
            sessionCtx->mStorageThread.join();
        }
        mStorageCache.erase(ctx);
    });
    context->mStorageThread = std::thread{[this, context]() {
        readSession(context);
    }};
    return result;
}

std::shared_ptr<GetSessionResult> SqliteSessionDatabase::getSessionByMetadataAsync(
    Common::SessionMetaData const& metadata) noexcept
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto result = std::make_shared<GetSessionResult>();
    auto context = std::make_shared<Private::SessionStorageContextWithValue<Common::SessionData>>(result);
    context->mSessionData = metadata;
    mStorageCache.emplace(context.get(), context);
    std::ignore = context->done.connect([this](Private::StorageContextBase* ctx) {
        auto sessionCtx =
            StorageContextBase::getStorageAs<SessionStorageContextWithValue<Common::SessionData>>(mStorageCache[ctx]);
        auto const result = sessionCtx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        if (result == System::Result::Ok) {
            auto const getResult = sessionCtx->getResultAs<GetSessionResult>();
            getResult->setResultValue(sessionCtx->value);
        }
        sessionCtx->mResult->setResult(result);
        mStorageCache.erase(ctx);
    });
    context->mStorageThread = std::thread{[this, context]() {
        readSessionByMetaData(context);
    }};
    return result;
}

std::shared_ptr<GetSessionMetaDataResult> SqliteSessionDatabase::getSessionMetaDataByIndexAsync(
    std::size_t index) noexcept
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto result = std::make_shared<GetSessionMetaDataResult>();
    auto context = std::make_shared<Private::SessionStorageContextWithValue<Common::SessionMetaData>>(result);
    context->mSessionId = index;
    mStorageCache.emplace(context.get(), context);
    std::ignore = context->done.connect([this](Private::StorageContextBase* ctx) {
        auto sessionCtx = StorageContextBase::getStorageAs<SessionStorageContextWithValue<Common::SessionMetaData>>(
            mStorageCache[ctx]);
        auto const result = sessionCtx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        if (result == System::Result::Ok) {
            auto const getResult = sessionCtx->getResultAs<GetSessionMetaDataResult>();
            getResult->setResultValue(sessionCtx->value);
        }
        sessionCtx->mResult->setResult(result);
        mStorageCache.erase(ctx);
        SPDLOG_INFO("SessionMetaData for session {} requested", sessionCtx->mSessionId);
    });
    context->mStorageThread = std::thread{[this, context] {
        readSessionMetaData(context);
    }};
    return result;
}

std::shared_ptr<System::AsyncResult> SqliteSessionDatabase::storeSession(Common::SessionData const& session)
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto sessionId = readSessionId(session);

    auto storageContext = std::make_shared<Private::SessionStorageContext>();
    mStorageCache.emplace(storageContext.get(), storageContext);
    storageContext->mStorageObject = session;
    storageContext->mSessionId = sessionId.value_or(0);

    auto storageOperationHandler = [this](Private::StorageContextBase* ctx) {
        auto sessionCtx = mStorageCache[ctx];
        auto const updateResult = sessionCtx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        sessionCtx->mResult->setResult(updateResult);
        if (updateResult == System::Result::Error) {
            SPDLOG_ERROR("Failed to store session with index {}. Error: {}",
                         sessionCtx->mSessionId,
                         mDbConnection->getErrorMessage());
        };

        if (sessionCtx->mStorageThread.joinable()) {
            sessionCtx->mStorageThread.join();
        }
        mStorageCache.erase(ctx);
        SPDLOG_INFO("Finished to store session {} from {} at {}",
                    sessionCtx->mStorageObject.getTrack().getTrackName(),
                    sessionCtx->mStorageObject.getSessionDate().asString(),
                    sessionCtx->mStorageObject.getSessionTime().asString());
    };

    std::ignore = storageContext->done.connect(storageOperationHandler);
    if (sessionId.has_value()) {
        storageContext->mIsUpdateContext = true;
        storageContext->mStorageThread = std::thread{&SqliteSessionDatabase::updateSession, this, storageContext.get()};
    } else {
        storageContext->mStorageThread = std::thread{&SqliteSessionDatabase::saveSession, this, storageContext.get()};
    }
    SPDLOG_INFO("Store session {} from {} at {}",
                session.getTrack().getTrackName(),
                session.getSessionDate().asString(),
                session.getSessionTime().asString());
    return storageContext->mResult;
}

void SqliteSessionDatabase::deleteSession(std::size_t index)
{
    std::lock_guard<std::mutex> const guard{mMutex};
    // clang-format off
    constexpr auto sessionDeleteQuery = "DELETE "
                                        "FROM "
                                            "Session "
                                        "WHERE "
                                            "Session.SessionId = ?";
    // clang-format on
    auto const sessionIndex = mIndexMapper.find(index);
    if (sessionIndex == mIndexMapper.cend()) {
        spdlog::error("Failed to delete session under index {} not found", index);
        return;
    }

    auto sessionDeleteStm = Statement{*mDbConnection};
    auto bindError =
        sessionDeleteStm.prepare(sessionDeleteQuery).bindValue(1, static_cast<int>(sessionIndex->second)).hasError();
    if (bindError or (sessionDeleteStm.execute() != ExecuteResult::Ok)) {
        spdlog::error("Failed to delete session under index {} Error: {}", index, mDbConnection->getErrorMessage());
    }
    updateIndexMapper();
    sessionDeleted.emit(index);
}

void SqliteSessionDatabase::updateSession(Private::SessionStorageContext* ctx)
{
    if (ctx == nullptr or mStorageCache.count(ctx) == 0) {
        SPDLOG_ERROR("Update session called with an invalid context.");
        return;
    }

    auto context = mStorageCache.at(ctx);
    // In the update case it's only necessary to add new laps to session if needed because other parts of a session
    // shouldn't be changed.
    auto const storedLaps = readLapsOfSession(context->mSessionId);
    if (!storedLaps.has_value()) {
        ctx->mStoragePromise.set_value(false);
        return;
    }

    auto const sessionLaps = context->mStorageObject.getLaps();
    if (sessionLaps.size() < storedLaps->size()) {
        ctx->mStoragePromise.set_value(true);
        return;
    }

    auto commitGuard = CommitGuard{*mDbConnection};
    for (std::size_t lapIndex = storedLaps->size(); lapIndex < sessionLaps.size(); ++lapIndex) {
        if (!saveLapOfSession(context->mSessionId, lapIndex, context->mStorageObject.getLaps().at(lapIndex))) {
            ctx->mStoragePromise.set_value(false);
            commitGuard.setRollback();
            return;
        }
        SPDLOG_DEBUG("Successful store lap {} of {} with ID {} for session with ID {}",
                     lapIndex,
                     sessionLaps.size(),
                     lapIndex,
                     ctx->mSessionId);
    }

    auto const updatedIndex = readIndexOfSessionId(context->mSessionId);
    if (!updatedIndex.has_value()) {
        ctx->mStoragePromise.set_value(false);
        commitGuard.setRollback();
        return;
    }
    ctx->mStoragePromise.set_value(true);
}

void SqliteSessionDatabase::saveSession(Private::SessionStorageContext* ctx)
{
    if (ctx == nullptr or mStorageCache.count(ctx) == 0) {
        spdlog::error("Update session called with an invalid context.");
        return;
    }

    auto context = mStorageCache.at(ctx);
    auto commitGuard = CommitGuard{*mDbConnection};
    // clang-format off
    constexpr auto insertQuery = "INSERT INTO SESSION (TrackId, Date, Time) "
                                 "VALUES "
                                 "((SELECT TrackId FROM Track WHERE Track.Name = ?), ?, ?)";
    // clang-format on

    // insert the session
    auto insertStm = Statement{*mDbConnection};
    auto bindError = insertStm.prepare(insertQuery)
                         .bindValue(1, ctx->mStorageObject.getTrack().getTrackName())
                         .bindValue(2, ctx->mStorageObject.getSessionDate().asString())
                         .bindValue(3, ctx->mStorageObject.getSessionTime().asString())
                         .hasError();
    if (bindError or (insertStm.execute() != ExecuteResult::Ok)) {
        SPDLOG_ERROR("Error insert session. Error:", mDbConnection->getErrorMessage());
        ctx->mStoragePromise.set_value(false);
        commitGuard.setRollback();
        return;
    }

    // get the session for inserting the laps.
    auto sessionId = readSessionId(ctx->mStorageObject);
    if (!sessionId.has_value()) {
        SPDLOG_ERROR("Failed to query session of new stored session");
        ctx->mStoragePromise.set_value(false);
        commitGuard.setRollback();
        return;
    }

    // insert the laps of the session
    auto const laps = ctx->mStorageObject.getLaps();
    for (std::size_t lapIndex = 0; lapIndex < laps.size(); ++lapIndex) {
        if (!saveLapOfSession(sessionId.value(), lapIndex, laps.at(lapIndex))) {
            ctx->mStoragePromise.set_value(false);
            commitGuard.setRollback();
            return;
        }
        SPDLOG_DEBUG("Successful store lap {} of {} with ID {} for session with ID {}",
                     lapIndex,
                     laps.size(),
                     lapIndex,
                     sessionId.value());
    }
    updateIndexMapper();
    ctx->mSessionId = sessionId.value();
    ctx->mStoragePromise.set_value(true);
}

void SqliteSessionDatabase::readSession(
    std::shared_ptr<Private::SessionStorageContextWithValue<Common::SessionData>> ctx) const
{
    auto session = readSession(ctx->mSessionId);
    auto success = false;
    if (session.has_value()) {
        ctx->value = session.value();
        success = true;
    }
    ctx->mStoragePromise.set_value(success);
}

void SqliteSessionDatabase::readSessionMetaData(
    std::shared_ptr<Private::SessionStorageContextWithValue<Common::SessionMetaData>> ctx) const
{
    auto maybeSessionMetaData = readSessionMetaData(ctx->mSessionId);
    auto success = false;
    if (maybeSessionMetaData.has_value()) {
        ctx->value = maybeSessionMetaData.value();
        success = true;
    }
    ctx->mStoragePromise.set_value(success);
}

void SqliteSessionDatabase::readSessionByMetaData(
    std::shared_ptr<Private::SessionStorageContextWithValue<Common::SessionData>> ctx) const
{
    auto maybeSessionData = readSessionByMetaData(ctx->mSessionData);
    auto success = false;
    if (maybeSessionData.has_value()) {
        ctx->value = maybeSessionData.value();
        success = true;
    }
    ctx->mStoragePromise.set_value(success);
}

std::optional<std::size_t> SqliteSessionDatabase::readSessionIdOfIndex(std::size_t sessionIndex) const noexcept
{
    auto const sessionIds = readSessionIds();
    if (sessionIndex > sessionIds.size()) {
        return std::nullopt;
    }

    return sessionIds[sessionIndex];
}

std::optional<std::size_t> SqliteSessionDatabase::readSessionId(Common::SessionData const& session) const noexcept
{
    // clang-format off
    constexpr auto sessionIdQuery = "SELECT "
                                        "Session.SessionId "
                                    "FROM "
                                        "Session "
                                    "WHERE "
                                        "Session.date = ? AND Session.time = ?";
    // clang-format on
    auto sessionIdStm = Statement{*mDbConnection};
    auto const bindError = sessionIdStm.prepare(sessionIdQuery)
                               .bindValue(1, session.getSessionDate().asString())
                               .bindValue(2, session.getSessionTime().asString())
                               .hasError();
    if (bindError) {
        spdlog::error("Error query session id. Error:", mDbConnection->getErrorMessage());
        return std::nullopt;
    }

    if ((sessionIdStm.execute() == ExecuteResult::Row) && (sessionIdStm.getColumn<int>(0).has_value())) {
        return static_cast<std::size_t>(sessionIdStm.getColumn<int>(0).value_or(0));
    }
    return std::nullopt;
}

std::optional<std::size_t> SqliteSessionDatabase::readIndexOfSessionId(std::size_t sessionId) const noexcept
{
    auto const sessionIds = readSessionIds();
    auto idIter = std::find_if(sessionIds.cbegin(), sessionIds.cend(), [sessionId](std::size_t id) {
        return id == sessionId;
    });
    if (idIter != sessionIds.cend()) {
        return std::distance(sessionIds.cbegin(), idIter);
    }
    return std::nullopt;
}

std::vector<std::size_t> SqliteSessionDatabase::readSessionIds() const noexcept
{
    // clang-format off
    constexpr auto sessionIdsQuery = "SELECT "
                                        "Session.SessionId "
                                    "FROM "
                                        "Session";
    // clang-format on
    auto sessionIdsStm = Statement{*mDbConnection};
    if (sessionIdsStm.prepare(sessionIdsQuery).hasError()) {
        spdlog::error("Failed to prepare query for session ids. Error: {}", mDbConnection->getErrorMessage());
        return {};
    }

    auto sessionIds = std::vector<std::size_t>();
    auto rowReadResult = ExecuteResult::Error;
    while (((rowReadResult = sessionIdsStm.execute()) == ExecuteResult::Row) && (sessionIdsStm.getColumnCount() > 0)) {
        auto const sessionIndex = sessionIdsStm.getColumn<int>(0);
        if (sessionIndex.has_value()) {
            sessionIds.push_back(*sessionIndex);
        } else {
            return {};
        }
    }

    return sessionIds;
}

std::optional<std::vector<Common::LapData>> SqliteSessionDatabase::readLapsOfSession(
    std::size_t sessionId) const noexcept
{
    // clang-format off
    constexpr auto lapIdQuery = "SELECT "
                                    "Lap.LapId "
                                "FROM "
                                    "Lap "
                                "WHERE "
                                    "Lap.SessionId = ?";
    constexpr auto sektorQuery = "SELECT "
                                    "SektorTime.Time "
                                 "FROM "
                                    "Session "
                                 "LEFT JOIN LAP ON "
                                    "Session.SessionId = Lap.SessionId "
                                 "LEFT JOIN SektorTime ON "
                                    "SektorTime.LapId = Lap.LapId "
                                 "WHERE "
                                     "Session.SessionId = ? AND SektorTime.LapId = ? ORDER BY SektorTime.SektorIndex AND Lap.LapIndex ASC";
    constexpr auto logPointQuery = "SELECT "
                                    "LogPoint.Longitude, LogPoint.Latitude, LogPoint.Velocity, LogPoint.Date, LogPoint.Time "
                                   "FROM "
                                    "LogPoint "
                                   "WHERE "
                                     "LogPoint.LapId = ? ORDER By LogPoint.Idx";
    // clang-format on
    auto lapIds = std::vector<std::size_t>{};
    auto lapIdStm = Statement{*mDbConnection};
    auto const bindError = lapIdStm.prepare(lapIdQuery).bindValue(1, static_cast<int>(sessionId)).hasError();
    if (bindError) {
        spdlog::error("Error prepare query lap ids. Error: {}", mDbConnection->getErrorMessage());
        return std::nullopt;
    }

    auto state = ExecuteResult::Error;
    while (((state = lapIdStm.execute()) == ExecuteResult::Row) && (lapIdStm.getColumnCount() > 0)) {
        auto lapId = lapIdStm.getColumn<int>(0);
        if (lapId.has_value()) {
            lapIds.emplace_back(lapId.value_or(0));
        }
    }

    if (state != ExecuteResult::Ok) {
        spdlog::error("Error query lap ids. Error:", mDbConnection->getErrorMessage());
        return std::nullopt;
    }

    auto laps = std::vector<Common::LapData>{};
    for (auto const& lapId : lapIds) {
        auto lapData = Common::LapData{};
        auto sektorStm = Statement{*mDbConnection};
        auto bindError = sektorStm.prepare(sektorQuery)
                             .bindValue(1, static_cast<int>(sessionId))
                             .bindValue(2, static_cast<int>(lapId))
                             .hasError();
        if (bindError) {
            spdlog::error("Error prepare lap query. Error: {}", mDbConnection->getErrorMessage());
            return std::nullopt;
        }

        while (((state = sektorStm.execute()) == ExecuteResult::Row) && (sektorStm.getColumnCount() > 0)) {
            auto const sektorTime = sektorStm.getColumn<std::string>(0);
            if (sektorTime.has_value()) {
                lapData.addSectorTime(Common::Timestamp{sektorTime.value_or("")});
            }
        }

        if (state != ExecuteResult::Ok) {
            spdlog::error("Error query lap ids. Error: {}", mDbConnection->getErrorMessage());
            return std::nullopt;
        }

        auto logPointStm = Statement{*mDbConnection};
        bindError = logPointStm.prepare(logPointQuery).bindValue(1, static_cast<int>(lapId)).hasError();
        if (bindError) {
            spdlog::error("Error prepare logpoint query. Error {}", mDbConnection->getErrorMessage());
            return std::nullopt;
        }

        while (((state = logPointStm.execute()) == ExecuteResult::Row) && (logPointStm.getColumnCount() > 0)) {
            auto const longitude = logPointStm.getColumn<float>(0);
            auto const latitude = logPointStm.getColumn<float>(1);
            auto const velocity = logPointStm.getColumn<float>(2);
            auto const date = logPointStm.getColumn<std::string>(3);
            auto const time = logPointStm.getColumn<std::string>(4);
            if (longitude.has_value() and latitude.has_value() and velocity.has_value() and date.has_value() and
                time.has_value()) {
                lapData.addPosition(Common::GpsPositionData{Common::PositionData{latitude.value(), longitude.value()},
                                                            Common::Timestamp{time.value()},
                                                            Common::Date{date.value()},
                                                            Common::VelocityData{velocity.value()}});
            }
        }
        laps.push_back(lapData);
    }

    return laps;
}

std::optional<Common::TrackData> SqliteSessionDatabase::readTrack(std::size_t trackId) const noexcept
{
    // clang-format off
    constexpr auto trackQuery =
        "SELECT TrackId, Track.Name, FL.Latitude AS FlLat, FL.Longitude AS FlLong, "
        "SL.Latitude AS SlLat, SL.Longitude AS SlLong from Track LEFT JOIN Position FL ON Track.Finishline = "
        "FL.PositionId LEFT JOIN Position SL ON Track.Startline = SL.PositionId WHERE Track.TrackId = ?";

    constexpr auto sektorQuery =
        "SELECT PO.Latitude, PO.Longitude FROM Track JOIN Sektor SE ON Track.TrackId = SE.TrackId JOIN "
        "Position PO ON SE.PositionId = PO.PositionId WHERE Track.TrackId = ? ORDER BY SE.SektorIndex ASC";
    // clang-format on
    Statement stm{*mDbConnection};
    auto bindError = stm.prepare(trackQuery).bindValue(1, static_cast<int>(trackId)).hasError();
    if (bindError or (stm.execute() != ExecuteResult::Row)) {
        spdlog::error("Error prepare track statement for id {}. Error {}", trackId, mDbConnection->getErrorMessage());
        return std::nullopt;
    }
    auto track = Rapid::Common::TrackData{};
    track.setTrackName(stm.getColumn<std::string>(1).value_or(""));
    track.setFinishline({stm.getColumn<float>(2).value_or(0), stm.getColumn<float>(3).value_or(0)});
    if (stm.hasColumnValue(4) == HasColumnValueResult::Ok && stm.hasColumnValue(5) == HasColumnValueResult::Ok) {
        track.setStartline({stm.getColumn<float>(4).value_or(0), stm.getColumn<float>(5).value_or(0)});
    }

    // Request sektor
    Statement sektorStm{*mDbConnection};
    bindError = sektorStm.prepare(sektorQuery).bindValue(1, static_cast<int>(trackId)).hasError();
    if (bindError) {
        return std::nullopt;
    }

    auto sections = std::vector<Rapid::Common::PositionData>{};
    while (sektorStm.execute() == ExecuteResult::Row && sektorStm.getColumnCount() == 2) {
        sections.emplace_back(sektorStm.getColumn<float>(0).value_or(0), sektorStm.getColumn<float>(1).value_or(0));
    }
    track.setSections(sections);
    return track;
}

bool SqliteSessionDatabase::saveLapOfSession(std::size_t sessionId,
                                             std::size_t lapIndex,
                                             Common::LapData const& lapData) const noexcept
{
    // clang-format off
    constexpr auto insertLapQuery = "INSERT INTO Lap(SessionId, LapIndex) "
                                    "VALUES "
                                    "(?, ?)";
    constexpr auto insetSektorQuery = "INSERT INTO SektorTime(LapId, Time, SektorIndex) "
                                      "VALUES "
                                      "(?, ?, ?)";
    // clang-format on
    auto lapInsertStm = Statement{*mDbConnection};
    auto bindError = lapInsertStm.prepare(insertLapQuery)
                         .bindValue(1, static_cast<int>(sessionId))
                         .bindValue(2, static_cast<int>(lapIndex))
                         .hasError();
    if (bindError or (lapInsertStm.execute() != ExecuteResult::Ok)) {
        spdlog::error("Error query session id. Error {}", mDbConnection->getErrorMessage());
        return false;
    }

    auto lapId = static_cast<int>(readLapId(sessionId, lapIndex).value_or(0));
    auto insertSektorStm = Statement{*mDbConnection};
    for (std::size_t sektorTimeIndex = 0; sektorTimeIndex < lapData.getSectorTimeCount(); ++sektorTimeIndex) {
        bindError = insertSektorStm.prepare(insetSektorQuery)
                        .bindValue(1, lapId)
                        .bindValue(2, lapData.getSectorTime(sektorTimeIndex).value_or(Common::Timestamp{}).asString())
                        .bindValue(3, static_cast<int>(sektorTimeIndex))
                        .hasError();
        if (bindError or (insertSektorStm.execute() != ExecuteResult::Ok)) {
            spdlog::error("Error failed to insert sektor. Error {}", mDbConnection->getErrorMessage());
            return false;
        }
    }

    if (!saveLapLogPoints(lapId, lapData)) {
        return false;
    }
    return true;
}

bool SqliteSessionDatabase::saveLapLogPoints(std::size_t lapId, Common::LapData const& lapData) const noexcept
{
    // clang-format off
    constexpr auto insertLogPoint = "INSERT INTO LogPoint(Idx, LapId, Velocity, Longitude, Latitude, Date, Time) "
                                    "VALUES "
                                    "(?, ?, ?, ?, ?, ?, ?)";
    // clang-format on
    auto const& positions = lapData.getPositions();
    auto insertLogPointStm = Statement{*mDbConnection};
    for (std::size_t idx = 0; idx < positions.size(); ++idx) {
        auto const gpsPos = positions.at(idx);
        auto const bindError = insertLogPointStm.prepare(insertLogPoint)
                                   .bindValue(1, static_cast<int>(idx))
                                   .bindValue(2, static_cast<int>(lapId))
                                   .bindValue(3, gpsPos.getVelocity().getVelocity())
                                   .bindValue(4, gpsPos.getPosition().getLongitude())
                                   .bindValue(5, gpsPos.getPosition().getLatitude())
                                   .bindValue(6, gpsPos.getDate().asString())
                                   .bindValue(7, gpsPos.getTime().asString())
                                   .hasError();
        if (bindError) {
            SPDLOG_ERROR("Failed to bind values LogPoint statement. Error: {}", mDbConnection->getErrorMessage());
            return false;
        }
        if (insertLogPointStm.execute() != ExecuteResult::Ok) {
            SPDLOG_ERROR("Failed to execute LogPoint statement. Error: {}", mDbConnection->getErrorMessage());
            return false;
        }
    }

    SPDLOG_DEBUG("Successful stored the log points for lap with ID {}", lapId);
    return true;
}

std::optional<std::size_t> SqliteSessionDatabase::readLapId(std::size_t sessionId, std::size_t lapIndex) const noexcept
{
    // clang-format off
    constexpr auto lapIdQuery = "SELECT Lap.LapId FROM Lap WHERE Lap.SessionId = ? AND Lap.LapIndex = ?";
    // clang-format on
    auto lapIdStm = Statement{*mDbConnection};
    auto const bindError = lapIdStm.prepare(lapIdQuery)
                               .bindValue(1, static_cast<int>(sessionId))
                               .bindValue(2, static_cast<int>(lapIndex))
                               .hasError();
    if (bindError) {
        spdlog::error("Faild to build prepare statement for lap ID. Error {}", mDbConnection->getErrorMessage());
        return std::nullopt;
    }
    if ((lapIdStm.execute() != ExecuteResult::Row) or (not lapIdStm.getColumn<int>(0).has_value())) {
        spdlog::error("Error failed to query lap id. Error {}", mDbConnection->getErrorMessage());
        return false;
    }
    return lapIdStm.getColumn<int>(0);
}

void SqliteSessionDatabase::handleUpdates(void* objPtr,
                                          int event,
                                          char const* database,
                                          char const* table,
                                          sqlite3_int64 rowId)
{
    auto sessionDatabase = static_cast<SqliteSessionDatabase*>(objPtr);
    constexpr auto sessionTable = "Session";
    constexpr auto lapTable = "Lap";
    if (std::strcmp(table, sessionTable) == 0) {
        switch (event) {
        case SQLITE_INSERT: {
            sessionDatabase->updateIndexMapper();
            auto index = std::find_if(sessionDatabase->mIndexMapper.cbegin(),
                                      sessionDatabase->mIndexMapper.cend(),
                                      [&rowId](auto const& entry) {
                                          return entry.second == static_cast<std::size_t>(rowId);
                                      });
            if (index != sessionDatabase->mIndexMapper.cend()) {
                SPDLOG_DEBUG("Session with index {} added", index->first);
                sessionDatabase->sessionAdded.emit(index->first);
            }
        } break;
        case SQLITE_DELETE: {
            for (auto const& [index, sessionId] : sessionDatabase->mIndexMapper) {
                if (sessionId == static_cast<std::size_t>(rowId)) {
                    sessionDatabase->sessionDeleted.emit(index);
                    sessionDatabase->mIndexMapper.erase(index);
                    break;
                }
            }
        } break;
        default:
            // do nothing
            break;
        }
    } else if (std::strcmp(table, lapTable) == 0) {
        switch (event) {
        case SQLITE_INSERT: {
            // clang-format off
        constexpr auto sessionIdQuery = "SELECT "
                                                                "Lap.SessionId "
                                                            "FROM "
                                                                "Lap "
                                                            "WHERE "
                                                                "rowid = ?";
            // clang-format on
            auto stm = Statement{*sessionDatabase->mDbConnection};
            auto bindError = stm.prepare(sessionIdQuery).bindValue(1, static_cast<int>(rowId)).hasError();
            if (bindError) {
                SPDLOG_ERROR("Failed to bind query for session update detection. Error: {}",
                             sessionDatabase->mDbConnection->getErrorMessage());
                return;
            }
            if (stm.execute() != ExecuteResult::Row) {
                return;
            }
            auto const sessionId = stm.getColumn<int>(0);
            auto index = std::find_if(sessionDatabase->mIndexMapper.cbegin(),
                                      sessionDatabase->mIndexMapper.cend(),
                                      [&sessionId](auto const& entry) {
                                          return entry.second == static_cast<std::size_t>(sessionId.value());
                                      });
            if (index != sessionDatabase->mIndexMapper.cend()) {
                SPDLOG_DEBUG("Session for index {} updated", index->first);
                sessionDatabase->sessionUpdated.emit(index->first);
            }
        } break;
        default:
            // do nothing
            break;
        }
    }
}

void SqliteSessionDatabase::updateIndexMapper()
{
    // clang-format off
    constexpr auto sessionIdsQuery = "SELECT "
                                          "Session.SessionId "
                                      "FROM "
                                          "Session "
                                      "ORDER BY "
                                          "Session.SessionId  "
                                      "ASC";
    // clang-format on
    auto sessionIdsStm = Statement{*mDbConnection};
    if (sessionIdsStm.prepare(sessionIdsQuery).hasError()) {
        spdlog::error("Failed to query session count. Error: {}", mDbConnection->getErrorMessage());
        return;
    }

    mIndexMapper.clear();
    auto executeResult = ExecuteResult::Error;
    auto index = std::size_t{0};
    while (((executeResult = sessionIdsStm.execute()) == ExecuteResult::Row) && (sessionIdsStm.getColumnCount() == 1)) {
        auto const sessionId = sessionIdsStm.getColumn<int>(0);
        if (sessionId.has_value()) {
            mIndexMapper.emplace(index, *sessionId);
            ++index;
        }
    }

    if (executeResult != ExecuteResult::Ok) {
        mIndexMapper.clear();
        spdlog::error("Failed to query all session ids. Error: {}", mDbConnection->getErrorMessage());
    }
}

std::optional<Common::SessionData> SqliteSessionDatabase::readSession(std::size_t index) const
{
    auto const sessionIndex = mIndexMapper.find(index);
    auto maybeSessionMetaData = readSessionMetaData(index);
    if (not maybeSessionMetaData.has_value()) {
        return std::nullopt;
    }

    auto laps = readLapsOfSession(sessionIndex->second);
    if (!laps.has_value()) {
        return std::nullopt;
    }

    auto sessionMetaData = maybeSessionMetaData.value();
    auto session = Common::SessionData{sessionMetaData.getTrack(),
                                       sessionMetaData.getSessionDate(),
                                       sessionMetaData.getSessionTime(),
                                       sessionMetaData.getId()};
    session.addLaps(laps.value_or(std::vector<Common::LapData>{}));
    return session;
}

std::optional<Common::SessionMetaData> SqliteSessionDatabase::readSessionMetaData(std::size_t index) const
{
    // clang-format off
    constexpr auto sessionQuery = "SELECT "
                                    "Session.Date, Session.Time, Session.TrackId "
                                  "FROM "
                                    "Session "
                                  "WHERE "
                                    "Session.SessionId = ?";
    // clang-format on
    auto const sessionIndex = mIndexMapper.find(index);
    if (sessionIndex == mIndexMapper.cend()) {
        return std::nullopt;
    }

    auto sessionStm = Statement{*mDbConnection};
    auto const bindError =
        sessionStm.prepare(sessionQuery).bindValue(1, static_cast<int>(sessionIndex->second)).hasError();
    if (bindError or (sessionStm.execute() != ExecuteResult::Row) or (sessionStm.getColumnCount() < 3)) {
        spdlog::error("Error query session. Error: {}", mDbConnection->getErrorMessage());
        return std::nullopt;
    }

    auto const trackId = static_cast<std::size_t>(sessionStm.getColumn<int>(2).value_or(0));
    auto trackData = readTrack(trackId);
    if (!trackData.has_value()) {
        return std::nullopt;
    }

    return Common::SessionMetaData{trackData.value_or(Common::TrackData{}),
                                   Common::Date{sessionStm.getColumn<std::string>(0).value_or("")},
                                   Common::Timestamp{sessionStm.getColumn<std::string>(1).value_or("")},
                                   sessionIndex->first};
}

std::optional<Common::SessionData> SqliteSessionDatabase::readSessionByMetaData(
    Common::SessionMetaData const& metadata) const
{
    // clang-format off
    constexpr auto sessionIdQuery = "SELECT "
                                    "Session.SessionId "
                                  "FROM "
                                    "Session "
                                  "WHERE "
                                  "Session.Date = ? AND Session.Time = ?";
    // clang-format on
    auto sessionIdQueryStm = Statement{*mDbConnection};
    auto bindError = sessionIdQueryStm.prepare(sessionIdQuery)
                         .bindValue(1, metadata.getSessionDate().asString())
                         .bindValue(2, metadata.getSessionTime().asString())
                         .hasError();
    if (bindError or (sessionIdQueryStm.execute() != ExecuteResult::Row) or (sessionIdQueryStm.getColumnCount() < 1)) {
        spdlog::error("Error query session id from meta data. Date: {} Time: {}. Error: {}",
                      mDbConnection->getErrorMessage(),
                      metadata.getSessionDate().asString(),
                      metadata.getSessionTime().asString());
        return std::nullopt;
    }
    auto maybeSessionId = sessionIdQueryStm.getColumn<int>(0);
    if (not maybeSessionId.has_value()) {
        spdlog::error("Column error query session id from meta data. Date: {} Time: {}. Error: {}",
                      mDbConnection->getErrorMessage(),
                      metadata.getSessionDate().asString(),
                      metadata.getSessionTime().asString());
        return std::nullopt;
    }
    auto sessionId = maybeSessionId.value();
    auto maybeIndex = getIndexForSessionId(sessionId);
    if (not maybeIndex.has_value()) {
        spdlog::error("No session index foud for session id {}", sessionId);
        return std::nullopt;
    }
    return readSession(maybeIndex.value());
}

std::optional<std::size_t> SqliteSessionDatabase::getIndexForSessionId(std::size_t sessionDbId) const
{
    auto sessionIndex = std::find_if(mIndexMapper.cbegin(), mIndexMapper.cend(), [sessionDbId](auto&& entry) {
        return entry.second == sessionDbId;
    });
    if (sessionIndex == mIndexMapper.cend()) {
        return std::nullopt;
    }
    return sessionIndex->first;
}

} // namespace Rapid::Storage
