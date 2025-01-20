// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidHeadless.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::LappyHeadless
{

LappyHeadless::LappyHeadless(Rapid::Positioning::IGpsPositionProvider& posProvider,
                             Rapid::Storage::ISessionDatabase& sessionDatabase,
                             Rapid::Storage::ITrackDatabase& trackDatabase)
    : mPositionProvider{posProvider}
    , mSessionDatabase{sessionDatabase}
    , mTrackDatabase{trackDatabase}
{
    std::ignore = mTrackDetectionWorkflow.trackDetected.connect([this] {
        auto const track = mTrackDetectionWorkflow.getDetectedTrack();
        spdlog::info("Track detected: {}", track.getTrackName());
        mTrackDetectionWorkflow.stopDetection();
        mActiveSessionWorkflow.setTrack(track);
        mActiveSessionWorkflow.startActiveSession();
    });

    std::ignore = mActiveSessionWorkflow.lapFinished.connect([this] {
        spdlog::info("Lap finished: {}", mActiveSessionWorkflow.lastLaptime.get().asString());
        spdlog::info("Lap count: {}", mActiveSessionWorkflow.lapCount.get());
    });

    mTrackDetectionWorkflow.setTracks(mTrackDatabase.getTracks());
    mTrackDetectionWorkflow.startDetection();

    mRestServer.registerGetHandler(std::string{"/sessions"}, &mSessionEndpoint);
    mRestServer.registerGetHandler(std::string{"/activeSession"}, std::addressof(mActiveSessionEndpoint));
    if (mRestServer.start() == Rest::ServerStartResult::Ok) {
        SPDLOG_INFO("Succesful start REST server");
    } else {
        SPDLOG_ERROR("Failed to start REST server");
    }
}

} // namespace Rapid::LappyHeadless
