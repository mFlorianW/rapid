// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidHeadless.hpp"

namespace Rapid::LappyHeadless
{

LappyHeadless::LappyHeadless(Rapid::Positioning::IGpsPositionProvider& posProvider,
                             Rapid::Storage::ISessionDatabase& sessionDatabase,
                             Rapid::Storage::ITrackDatabase& trackDatabase)
    : mPositionProvider{posProvider}
    , mSessionDatabase{sessionDatabase}
    , mTrackDatabase{trackDatabase}
{
    mTrackDetectionWorkflow.trackDetected.connect([this] {
        auto const track = mTrackDetectionWorkflow.getDetectedTrack();
        std::cout << "Track detected:" << track.getTrackName() << "\n";
        mTrackDetectionWorkflow.stopDetection();
        mActiveSessionWorkflow.setTrack(track);
        mActiveSessionWorkflow.startActiveSession();
    });

    mActiveSessionWorkflow.lapFinished.connect([this] {
        std::cout << "Lap finished:" << mActiveSessionWorkflow.lastLaptime.get().asString() << "\n";
        std::cout << "Lap count:" << mActiveSessionWorkflow.lapCount.get() << "\n";
    });

    mTrackDetectionWorkflow.setTracks(mTrackDatabase.getTracks());
    mTrackDetectionWorkflow.startDetection();

    mRestServer.registerGetHandler(std::string{"/sessions"}, &mSessionEndpoint);
    (void)mRestServer.start();
}

} // namespace Rapid::LappyHeadless
