// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidHeadless.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::LappyHeadless
{

LappyHeadless::LappyHeadless(Rapid::Positioning::IGpsPositionProvider& posProvider,
                             Rapid::Positioning::IGpsInformationProvider& gpsInfoProvider,
                             Rapid::Storage::ISessionDatabase& sessionDatabase,
                             Rapid::Storage::ITrackDatabase& trackDatabase)
    : mPositionProvider{posProvider}
    , mGpsInfoProvider{gpsInfoProvider}
    , mSessionDatabase{sessionDatabase}
    , mTrackDatabase{trackDatabase}
    , mGpsRestResource{&mGpsInfoProvider, &mPositionProvider}
{
    mTrackDetectionConnection = mTrackDetectionWorkflow.trackDetected.connect([this] {
        auto const track = mTrackDetectionWorkflow.getDetectedTrack();
        spdlog::info("Track detected: {}", track.getTrackName());
        mTrackDetectionWorkflow.stopDetection();
        mActiveSessionWorkflow.setTrack(track);
        mTrackDetected = true;
        startSession();
    });

    mGpsFixModeConnection = mGpsInfoProvider.gpsFixModeChanged.connect([this](auto&& mode) {
        hasFix(mode);
    });

    mLapFinishedConnection = mActiveSessionWorkflow.lapFinished.connect([this] {
        spdlog::info("Lap finished: {}", mActiveSessionWorkflow.lastLaptime.get().asString());
        spdlog::info("Lap count: {}", mActiveSessionWorkflow.lapCount.get());
    });

    mTrackDetectionWorkflow.setTracks(mTrackDatabase.getTracks());
    mTrackDetectionWorkflow.startDetection();

    mRestServer.registerGetHandler(std::string{"/sessions"}, &mSessionEndpoint);
    mRestServer.registerGetHandler(std::string{"/activeSession"}, std::addressof(mActiveSessionEndpoint));
    mRestServer.registerGetHandler(std::string{"/gps"}, std::addressof(mGpsRestResource));
    if (mRestServer.start() == Rest::ServerStartResult::Ok) {
        SPDLOG_INFO("Succesful start REST server");
    } else {
        SPDLOG_ERROR("Failed to start REST server");
    }

    auto const mode = mGpsInfoProvider.getGpsFixMode();
    hasFix(mode);
}

void LappyHeadless::hasFix(Rapid::Positioning::GpsFixMode mode)
{
    if (mode == Positioning::GpsFixMode::Fix2d or mode == Positioning::GpsFixMode::Fix3d) {
        mHasFix = true;
        startSession();
    }
}

void LappyHeadless::startSession()
{
    if (mTrackDetected and mHasFix and not mActiveSessionWorkflow.getSession().has_value()) {
        mActiveSessionWorkflow.startActiveSession();
        auto const session = mActiveSessionWorkflow.getSession().value_or(Common::SessionData{});
        SPDLOG_INFO("Active Session started on {} {}",
                    session.getSessionDate().asString(),
                    session.getSessionTime().asString());
    }
}

} // namespace Rapid::LappyHeadless
