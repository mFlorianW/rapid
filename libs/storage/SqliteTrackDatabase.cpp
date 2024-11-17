// SPDX-FileCopyrightText: 2024 All contributors
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
}

SqliteTrackDatabase::~SqliteTrackDatabase() = default;

std::size_t SqliteTrackDatabase::getTrackCount()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    constexpr auto statementStr = "SELECT COUNT(TrackId) FROM Track";
    Statement stm{mDbConnection};
    if (stm.prepare(statementStr).hasError() or stm.execute() != ExecuteResult::Row or stm.getColumnCount() == 0) {
        spdlog::error("Database Error: {}", mDbConnection.getErrorMessage());
        return 0;
    }

    return stm.getColumn<int>(0).value_or(0);
}

std::vector<Common::TrackData> SqliteTrackDatabase::getTracks()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    constexpr auto trackQuery =
        "SELECT TrackId, Track.Name, FL.Latitude AS FlLat, FL.Longitude AS FlLong, "
        "SL.Latitude AS SlLat, SL.Longitude AS SlLong from Track LEFT JOIN Position FL ON Track.Finishline = "
        "FL.PositionId LEFT JOIN Position SL ON Track.Startline = SL.PositionId";
    auto tracksResult = std::vector<Common::TrackData>{};

    Statement stm{mDbConnection};
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
            Statement sektorStm{mDbConnection};
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
    }
    return tracksResult;
}

std::shared_ptr<System::AsyncResult> SqliteTrackDatabase::saveTrack(std::vector<Common::TrackData> const& tracks)
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto context = std::make_shared<TrackStorageContext>();
    mStorageCache.insert({context.get(), context});
    return context->mResult;
}

std::shared_ptr<System::AsyncResult> SqliteTrackDatabase::deleteTrack(std::size_t trackIndex)
{
    auto const guard = std::lock_guard<std::mutex>{mMutex};
    auto context = std::make_shared<TrackStorageContext>();
    mStorageCache.insert({context.get(), context});
    context->mTrackIndex = trackIndex;
    context->done.connect([this](StorageContextBase* baseCtx) {
        auto const updateResult = baseCtx->mStorageResult.getResult() ? System::Result::Ok : System::Result::Error;
        baseCtx->mResult->setDbResult(updateResult);
        if (baseCtx->mStorageThread.joinable()) {
            baseCtx->mStorageThread.join();
        }
        mStorageCache.erase(baseCtx);
    });
    context->mStorageThread = std::thread{&SqliteTrackDatabase::removeOneTrack, this, context.get()};
    return context->mResult;
}

std::shared_ptr<System::AsyncResult> SqliteTrackDatabase::deleteAllTracks()
{
    std::lock_guard<std::mutex> const guard{mMutex};
    auto context = std::make_shared<TrackStorageContext>();
    mStorageCache.insert({context.get(), context});
    return context->mResult;
}

void SqliteTrackDatabase::removeOneTrack(Private::TrackStorageContext* ctx)
{
    if (ctx == nullptr or mStorageCache.count(ctx) == 0) {
        spdlog::error("Update session called with an invalid context.");
        return;
    }

    auto context = mStorageCache.at(ctx);
    // clang-format off
    constexpr auto deleteTrackQuery = "DELETE "
                                     "FROM "
                                        "TRACK "
                                     "WHERE "
                                        "Track.TrackId = ?";
    // clang-format on
    auto const trackId = getTrackIdOfIndex(context->mTrackIndex);
    if (!trackId.has_value()) {
        spdlog::error("Failed to delete Track. Index {} not found", context->mTrackIndex);
        context->mStoragePromise.set_value(false);
        return;
    }

    auto deleteTrackStm = Statement{mDbConnection};
    auto const bindError = deleteTrackStm.prepare(deleteTrackQuery).bindValue(1, static_cast<int>(*trackId)).hasError();
    if (bindError or (deleteTrackStm.execute() != ExecuteResult::Ok)) {
        spdlog::error("Failed to delete track. Error: {}", mDbConnection.getErrorMessage());
        context->mStoragePromise.set_value(false);
        return;
    }
    context->mStoragePromise.set_value(true);
}

std::vector<std::size_t> SqliteTrackDatabase::getTrackIds() const noexcept
{
    // clang-format off
    constexpr auto trackIdQuery = "SELECT "
                                    "Track.TrackId "
                                  "FROM "
                                    "Track";
    // clang-format on
    auto trackIdStm = Statement{mDbConnection};
    if (trackIdStm.prepare(trackIdQuery).hasError()) {
        spdlog::error("Failed to prepare track id query. Error: {}", mDbConnection.getErrorMessage());
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

std::optional<std::size_t> SqliteTrackDatabase::getTrackIdOfIndex(std::size_t trackIndex) const noexcept
{
    auto const trackIds = getTrackIds();
    if (trackIndex > trackIds.size()) {
        return std::nullopt;
    }

    return trackIds[trackIndex];
}

} // namespace Rapid::Storage
