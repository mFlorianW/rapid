// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SqliteTrackDatabase.hpp"
#include "private/Statement.hpp"
#include <spdlog/spdlog.h>
#include <string>

using namespace Rapid::Storage::Private;

namespace Rapid::Storage
{

SqliteTrackDatabase::SqliteTrackDatabase(std::string const& pathToDatabase)
    : mDbConnection{Connection::connection(pathToDatabase)}
{
    auto* rawHandle = mDbConnection->getRawHandle();
    sqlite3_update_hook(rawHandle, &SqliteTrackDatabase::handleUpdates, this);
    updateIndexMapper();
}

SqliteTrackDatabase::~SqliteTrackDatabase() = default;

std::size_t SqliteTrackDatabase::getTrackCount()
{
    auto const trackCount = readTrackCount();
    if (trackCount.has_value()) {
        return trackCount.value();
    }
    return 0;
}

std::shared_ptr<AsyncTrackCountResult> SqliteTrackDatabase::getTrackCountAsync()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto result = std::make_shared<AsyncTrackCountResult>();
    auto context = std::make_shared<TrackStorageContextWithValue<std::size_t>>(result);
    mStorageCache.insert({context.get(), context});
    std::ignore = context->done.connect([this](StorageContextBase* ctx) {
        auto const updateResult = ctx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        auto const context =
            StorageContextBase::getStorageAs<TrackStorageContextWithValue<std::size_t>>(mStorageCache.at(ctx));
        ctx->getResultAs<AsyncTrackCountResult>()->setResultValue(context->value);
        ctx->mResult->setResult(updateResult);
        if (ctx->mStorageThread.joinable()) {
            ctx->mStorageThread.join();
        }
        mStorageCache.erase(ctx);
    });
    context->mStorageThread = std::thread{[this, context]() {
        readTrackCountAsync(context);
    }};
    return result;
}

std::vector<Common::TrackData> SqliteTrackDatabase::getTracks()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto tracks = readTracks();
    if (tracks.has_value()) {
        return tracks.value();
    }
    return {};
}

std::shared_ptr<AsyncTrackResult> SqliteTrackDatabase::getTracksAsync()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto result = std::make_shared<AsyncTrackResult>();
    auto context = std::make_shared<GetTrackContext>(result);
    mStorageCache.insert({context.get(), context});
    std::ignore = context->done.connect([this](StorageContextBase* ctx) {
        auto const updateResult = ctx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        auto const context = StorageContextBase::getStorageAs<GetTrackContext>(mStorageCache.at(ctx));
        ctx->getResultAs<AsyncTrackResult>()->setResultValue(context->value);
        ctx->mResult->setResult(updateResult);
        if (ctx->mStorageThread.joinable()) {
            ctx->mStorageThread.join();
        }
        mStorageCache.erase(ctx);
    });
    context->mStorageThread = std::thread{[this, context]() {
        readTracksAsync(context);
    }};
    return result;
}

std::shared_ptr<System::AsyncResult> SqliteTrackDatabase::saveTrack(Common::TrackData const& track)
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto context = std::make_shared<TrackStorageContext>();
    mStorageCache.insert({context.get(), context});
    std::ignore = context->done.connect([this](StorageContextBase* ctx) {
        auto const updateResult = ctx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        ctx->mResult->setResult(updateResult);
        if (ctx->mStorageThread.joinable()) {
            ctx->mStorageThread.join();
        }
        mStorageCache.erase(ctx);
    });
    context->mStorageObject = track;
    context->mStorageThread = std::thread{[this, context]() {
        saveTrack(context);
    }};
    return context->mResult;
}

std::shared_ptr<System::AsyncResult> SqliteTrackDatabase::deleteTrack(std::size_t trackIndex)
{
    auto const guard = std::lock_guard<std::mutex>{mMutex};
    auto context = std::make_shared<TrackStorageContext>();
    mStorageCache.insert({context.get(), context});
    context->mTrackIndex = trackIndex;
    std::ignore = context->done.connect([this](StorageContextBase* baseCtx) {
        auto const updateResult = baseCtx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        baseCtx->mResult->setResult(updateResult);
        if (baseCtx->mStorageThread.joinable()) {
            baseCtx->mStorageThread.join();
        }
        mStorageCache.erase(baseCtx);
    });
    context->mStorageThread = std::thread{[this, context]() {
        deleteTrack(context);
    }};
    return context->mResult;
}

std::shared_ptr<System::AsyncResult> SqliteTrackDatabase::deleteAllTracks()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto context = std::make_shared<TrackStorageContext>();
    mStorageCache.insert({context.get(), context});
    mStorageCache.insert({context.get(), context});
    std::ignore = context->done.connect([this](StorageContextBase* baseCtx) {
        auto const updateResult = baseCtx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        baseCtx->mResult->setResult(updateResult);
        if (baseCtx->mStorageThread.joinable()) {
            baseCtx->mStorageThread.join();
        }
        mStorageCache.erase(baseCtx);
    });
    context->mStorageThread = std::thread{[this, context]() {
        deleteAllTracks(context);
    }};
    return context->mResult;
}

void SqliteTrackDatabase::deleteTrack(std::shared_ptr<Private::TrackStorageContext> ctx)
{
    // clang-format off
    constexpr auto deleteTrackQuery = "DELETE "
                                     "FROM "
                                        "TRACK "
                                     "WHERE "
                                        "Track.TrackId = ?";
    // clang-format on
    auto const trackId = readTrackIdOfIndex(ctx->mTrackIndex);
    if (!trackId.has_value()) {
        spdlog::error("Failed to delete Track. Index {} not found", ctx->mTrackIndex);
        ctx->mStoragePromise.set_value(false);
        return;
    }

    auto deleteTrackStm = Statement{*mDbConnection};
    auto const bindError = deleteTrackStm.prepare(deleteTrackQuery).bindValue(1, static_cast<int>(*trackId)).hasError();
    if (bindError or (deleteTrackStm.execute() != ExecuteResult::Ok)) {
        SPDLOG_ERROR("Failed to delete track. Error: {}", mDbConnection->getErrorMessage());
        ctx->mStoragePromise.set_value(false);
        return;
    }
    ctx->mStoragePromise.set_value(true);
}

void SqliteTrackDatabase::saveTrack(std::shared_ptr<Private::TrackStorageContext> ctx)
{
    auto const track = ctx->mStorageObject;
    auto commitGuard = CommitGuard{*mDbConnection};
    auto const finishlineId = savePosition(track.getFinishline());
    if (not finishlineId.has_value()) {
        SPDLOG_ERROR("Failed to save track finish line. Error: {}", mDbConnection->getErrorMessage());
        commitGuard.setRollback();
        ctx->mStoragePromise.set_value(false);
        return;
    }

    auto const startlinePos = ctx->mStorageObject.getStartline();
    auto startlineId = std::optional<std::size_t>();
    if (startlinePos.getLongitude() > 0 && startlinePos.getLatitude() > 0) {
        startlineId = savePosition(startlinePos);
        if (not startlineId.has_value()) {
            SPDLOG_ERROR("Failed to save track start line. Error: {}", mDbConnection->getErrorMessage());
            ctx->mStoragePromise.set_value(false);
            return;
        }
    }

    auto const trackId = saveTrack(track.getTrackName(), finishlineId.value(), startlineId);
    if (not trackId.has_value()) {
        SPDLOG_ERROR("Failed to save track. Error: {}", mDbConnection->getErrorMessage());
        commitGuard.setRollback();
        ctx->mStoragePromise.set_value(false);
        return;
    }

    auto const sections = track.getSections();
    for (std::size_t index = 0; index < sections.size(); ++index) {
        if (not saveSection(trackId.value(), sections.at(index), index)) {
            commitGuard.setRollback();
            SPDLOG_ERROR("Failed to save section of track. Error {}", mDbConnection->getErrorMessage());
        }
    }
    ctx->mStoragePromise.set_value(true);
}

void SqliteTrackDatabase::deleteAllTracks(std::shared_ptr<Private::TrackStorageContext> ctx)
{
    constexpr auto deleteAllTrackQuery = "DELETE FROM Track";

    auto const trackIds = readTrackIds();
    auto positionIds = std::vector<std::size_t>{};
    for (auto const& trackId : trackIds) {
        auto finishlineId = readFinishlinePositionId(trackId);
        auto startlineId = readStartlinePositionId(trackId);
        auto sectionIds = getSectionPositionIds(trackId);
        if (finishlineId.has_value()) {
            positionIds.push_back(finishlineId.value());
        }

        if (startlineId.has_value()) {
            positionIds.push_back(startlineId.value());
        }

        if (sectionIds.size() > 0) {
            positionIds.insert(positionIds.end(), sectionIds.cbegin(), sectionIds.cend());
        }
    }

    for (auto const& pos : positionIds) {
        if (not deletePositionId(pos)) {
            ctx->mStoragePromise.set_value(false);
        }
    }

    auto stm = Statement{*mDbConnection};
    if (stm.prepare(deleteAllTrackQuery).hasError() or stm.execute() != ExecuteResult::Ok) {
        ctx->mStoragePromise.set_value(false);
    }
    ctx->mStoragePromise.set_value(true);
}

void SqliteTrackDatabase::readTrackCountAsync(std::shared_ptr<Private::TrackStorageContextWithValue<std::size_t>> ctx)
{
    auto trackCount = readTrackCount();
    auto success = false;
    if (trackCount.has_value()) {
        ctx->value = trackCount.value();
        success = true;
    }
    ctx->mStoragePromise.set_value(success);
}

void SqliteTrackDatabase::readTracksAsync(std::shared_ptr<GetTrackContext> ctx)
{
    auto tracks = readTracks();
    auto success = false;
    if (tracks.has_value()) {
        ctx->value = tracks.value();
        success = true;
    }
    ctx->mStoragePromise.set_value(success);
}

std::vector<std::size_t> SqliteTrackDatabase::readTrackIds() const noexcept
{
    // clang-format off
    constexpr auto trackIdQuery = "SELECT "
                                    "Track.TrackId "
                                  "FROM "
                                    "Track";
    // clang-format on
    auto trackIdStm = Statement{*mDbConnection};
    if (trackIdStm.prepare(trackIdQuery).hasError()) {
        SPDLOG_ERROR("Failed to prepare track id query. Error: {}", mDbConnection->getErrorMessage());
        return {};
    }

    auto executeResult = ExecuteResult::Error;
    auto trackIds = std::vector<std::size_t>{};
    while (((executeResult = trackIdStm.execute()) == ExecuteResult::Row) && (trackIdStm.getColumnCount() > 0)) {
        auto const trackId = trackIdStm.getColumn<int>(0);
        if (trackId.has_value()) {
            trackIds.push_back(*trackId);
        } else {
            return {};
        }
    }

    return trackIds;
}

std::optional<std::size_t> SqliteTrackDatabase::readTrackIdOfIndex(std::size_t trackIndex) const noexcept
{
    auto const trackIds = readTrackIds();
    if (trackIndex > trackIds.size()) {
        return std::nullopt;
    }

    return trackIds[trackIndex];
}

std::optional<std::size_t> SqliteTrackDatabase::savePosition(Common::PositionData const& position) const noexcept
{
    // clang-format off
    constexpr auto storePositionQuery = "INSERT INTO Position "
                                            "(Longitude, Latitude) "
                                        "VALUES "
                                            "(?,?) "
                                        "RETURNING "
                                            "PositionId";

    // clang-format on
    auto positionStm = Statement{*mDbConnection};
    auto bindError = positionStm.prepare(storePositionQuery)
                         .bindValue(1, position.getLongitude())
                         .bindValue(2, position.getLatitude())
                         .hasError();
    if (bindError or positionStm.execute() != ExecuteResult::Row) {
        return std::nullopt;
    }
    return positionStm.getColumn<int>(0);
}

std::optional<std::size_t> SqliteTrackDatabase::saveTrack(std::string const& name,
                                                          std::size_t finishline,
                                                          std::optional<std::size_t> startline) const noexcept
{
    // clang-format off
    constexpr auto insertTrackWithStartlineQuery =  "INSERT INTO Track "
                                                        "(Name, Finishline, Startline) "
                                                    "VALUES "
                                                        "(?,?,?) "
                                                    "RETURNING "
                                                        "TrackId";
    constexpr auto insertTrackWithoutStartlineQuery =   "INSERT INTO Track "
                                                            "(Name, Finishline) "
                                                        "VALUES "
                                                            "(?,?) "
                                                        "RETURNING "
                                                            "TrackId";
    // clang-format on

    auto stm = Statement{*mDbConnection};
    auto bindError = false;
    if (startline.has_value()) {
        bindError = stm.prepare(insertTrackWithStartlineQuery)
                        .bindValue(1, name)
                        .bindValue(2, finishline)
                        .bindValue(3, startline.value())
                        .hasError();
    } else {
        bindError =
            stm.prepare(insertTrackWithoutStartlineQuery).bindValue(1, name).bindValue(2, finishline).hasError();
    }

    if (bindError or stm.execute() != ExecuteResult::Row) {
        return std::nullopt;
    }
    return stm.getColumn<int>(0);
}

bool SqliteTrackDatabase::saveSection(std::size_t trackId, Common::PositionData const& section, std::size_t index)
{
    // clang-format off
    constexpr auto insertSectionQuery = "INSERT INTO Sektor "
                                            "(PositionId, TrackId, SektorIndex) "
                                        "VALUES "
                                            "(?,?,?)";
    // clang-format on
    auto positionId = savePosition(section);
    if (not positionId.has_value()) {
        return false;
    }

    auto stm = Statement{*mDbConnection};
    auto bindError = stm.prepare(insertSectionQuery)
                         .bindValue(1, positionId.value())
                         .bindValue(2, trackId)
                         .bindValue(3, index)
                         .hasError();
    if (bindError or stm.execute() != ExecuteResult::Ok) {
        return false;
    }
    return true;
}

std::optional<std::size_t> SqliteTrackDatabase::readFinishlinePositionId(std::size_t trackId) const noexcept
{
    // clang-format off
    constexpr auto finishlinePositionIdQuery =
                                    "SELECT "
                                        "PositionId "
                                    "FROM "
                                        "Position "
                                    "WHERE "
                                        "PositionId = "
                                            "(SELECT Track.Finishline FROM Track WHERE TrackId = ?)";
    // clang-format on
    auto stm = Statement{*mDbConnection};
    auto const bindError = stm.prepare(finishlinePositionIdQuery).bindValue(1, trackId).hasError();
    if (bindError or stm.execute() != ExecuteResult::Row) {
        return std::nullopt;
    }
    return stm.getColumn<int>(0);
}

std::optional<std::size_t> SqliteTrackDatabase::readStartlinePositionId(std::size_t trackId) const noexcept
{
    // clang-format off
    constexpr auto startLinePositionIdQuery =
                                        "SELECT "
                                            "PositionId "
                                        "FROM "
                                            "Position "
                                        "WHERE "
                                            "PositionId = "
                                                "(SELECT Track.Startline FROM Track WHERE TrackId = ?)";
    // clang-format on
    auto stm = Statement{*mDbConnection};
    auto const bindError = stm.prepare(startLinePositionIdQuery).bindValue(1, trackId).hasError();
    if (bindError or stm.execute() != ExecuteResult::Row) {
        return std::nullopt;
    }
    return stm.getColumn<int>(0);
}

bool SqliteTrackDatabase::deletePositionId(std::size_t positionId)
{
    // clang-format off
    constexpr auto deletePositionQuery =
                                    "DELETE "
                                    "FROM "
                                        "POSITION "
                                    "WHERE "
                                        "PositionId = ?";
    // clang-format onn

    auto stm = Statement{*mDbConnection};
    auto const bindError = stm.prepare(deletePositionQuery).bindValue(1, positionId).hasError();
    if (bindError or stm.execute() != ExecuteResult::Ok) {
        return true;
    }
    return true;
}

std::vector<std::size_t> SqliteTrackDatabase::getSectionPositionIds(std::size_t trackId)
{
    // clang-format off
    constexpr auto sectionIdsQuery =
        "SELECT PositionId FROM Sektor WHERE TrackId = ?";
    // clang-format on
    auto stm = Statement{*mDbConnection};
    auto const bindError = stm.prepare(sectionIdsQuery).bindValue(1, trackId).hasError();
    if (bindError or stm.execute() != ExecuteResult::Row) {
        return {};
    }
    auto ids = std::vector<std::size_t>{};

    do {
        auto const id = stm.getColumn<int>(0);
        if (id.has_value()) {
            ids.push_back(id.value());
        }
    } while (stm.execute() == ExecuteResult::Row && stm.getColumnCount() == 1);
    return ids;
}

std::optional<std::size_t> SqliteTrackDatabase::readTrackCount()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    constexpr auto statementStr = "SELECT COUNT(TrackId) FROM Track";
    Statement stm{*mDbConnection};
    if (stm.prepare(statementStr).hasError() or stm.execute() != ExecuteResult::Row or stm.getColumnCount() == 0) {
        SPDLOG_ERROR("Database Error: {}", mDbConnection->getErrorMessage());
        return std::nullopt;
    }
    return stm.getColumn<int>(0).value_or(0);
}

std::optional<std::vector<Common::TrackData>> SqliteTrackDatabase::readTracks()
{
    constexpr auto trackQuery =
        "SELECT TrackId, Track.Name, FL.Latitude AS FlLat, FL.Longitude AS FlLong, "
        "SL.Latitude AS SlLat, SL.Longitude AS SlLong from Track LEFT JOIN Position FL ON Track.Finishline = "
        "FL.PositionId LEFT JOIN Position SL ON Track.Startline = SL.PositionId";

    auto tracksResult = std::vector<Common::TrackData>{};
    Statement stm{*mDbConnection};
    if (not stm.prepare(trackQuery).hasError()) {
        while (stm.execute() == ExecuteResult::Row && stm.getColumnCount() == 6) {
            auto track = Rapid::Common::TrackData{};
            auto trackId = stm.getColumn<int>(0).value_or(0);
            track.setTrackName(stm.getColumn<std::string>(1).value_or(""));
            track.setFinishline({stm.getColumn<float>(2).value_or(0), stm.getColumn<float>(3).value_or(0)});
            if (stm.hasColumnValue(4) == HasColumnValueResult::Ok &&
                stm.hasColumnValue(5) == HasColumnValueResult::Ok) {
                track.setStartline({stm.getColumn<float>(4).value_or(0), stm.getColumn<float>(5).value_or(0)});
            }

            // Request sektor
            constexpr auto sektorQuery =
                "SELECT PO.Latitude, PO.Longitude FROM Track JOIN Sektor SE ON Track.TrackId = SE.TrackId JOIN "
                "Position PO ON SE.PositionId = PO.PositionId WHERE Track.TrackId = ? ORDER BY SE.SektorIndex ASC";
            Statement sektorStm{*mDbConnection};
            auto const bindError = sektorStm.prepare(sektorQuery).bindValue(1, trackId).hasError();
            if (bindError) {
                tracksResult.clear();
                break;
            }

            auto sections = std::vector<Rapid::Common::PositionData>{};
            while (sektorStm.execute() == ExecuteResult::Row && sektorStm.getColumnCount() == 2) {
                sections.emplace_back(sektorStm.getColumn<float>(0).value_or(0),
                                      sektorStm.getColumn<float>(1).value_or(0));
            }
            track.setSections(sections);
            tracksResult.emplace_back(track);
        }
    } else {
        return std::nullopt;
    }
    return tracksResult;
}

bool SqliteTrackDatabase::updateIndexMapper()
{
    // clang-format off
    constexpr auto trackIdQuery = "SELECT "
                                          "Track.TrackId "
                                      "FROM "
                                          "Track "
                                      "ORDER BY "
                                          "Track.TrackId  "
                                      "ASC";
    // clang-format on
    auto trackIdStm = Statement{*mDbConnection};
    if (trackIdStm.prepare(trackIdQuery).hasError()) {
        SPDLOG_ERROR("Failed to query track id count. Error: {}", mDbConnection->getErrorMessage());
        return false;
    }
    mIndexMapper.clear();
    auto executeResult = ExecuteResult::Error;
    auto index = std::size_t{0};
    while (((executeResult = trackIdStm.execute()) == ExecuteResult::Row) && (trackIdStm.getColumnCount() == 1)) {
        auto const trackId = trackIdStm.getColumn<int>(0);
        if (trackId.has_value()) {
            mIndexMapper.emplace(index, *trackId);
            ++index;
        }
    }

    if (executeResult != ExecuteResult::Ok) {
        mIndexMapper.clear();
        SPDLOG_ERROR("Failed to query all session ids. Error: {}", mDbConnection->getErrorMessage());
    }
    return true;
}

void SqliteTrackDatabase::handleUpdates(void* objPtr,
                                        int event,
                                        char const* database,
                                        char const* table,
                                        sqlite3_int64 rowId)
{
    constexpr auto trackTable = "Track";
    if (std::strcmp(table, trackTable) != 0) {
        return;
    }
    auto* trackDatabase = static_cast<SqliteTrackDatabase*>(objPtr);
    switch (event) {
    case SQLITE_INSERT: {
        trackDatabase->updateIndexMapper();
        auto index = std::find_if(trackDatabase->mIndexMapper.cbegin(),
                                  trackDatabase->mIndexMapper.cend(),
                                  [&rowId](auto const& entry) {
                                      return entry.second == static_cast<std::size_t>(rowId);
                                  });
        if (index != trackDatabase->mIndexMapper.cend()) {
            trackDatabase->trackAdded.emit(index->first);
        }
    } break;
    case SQLITE_DELETE: {
        auto index = std::find_if(trackDatabase->mIndexMapper.cbegin(),
                                  trackDatabase->mIndexMapper.cend(),
                                  [&rowId](auto const& entry) {
                                      return entry.second == static_cast<std::size_t>(rowId);
                                  });
        if (index != trackDatabase->mIndexMapper.cend()) {
            trackDatabase->trackDeleted.emit(index->first);
            trackDatabase->mIndexMapper.erase(index);
        };
        trackDatabase->updateIndexMapper();
    } break;
    default:
        //do nothing
        break;
    }
}

} // namespace Rapid::Storage
