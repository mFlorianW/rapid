// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ActiveSessionModel.hpp"

#include "TrackData.hpp"

using namespace Rapid::Common;

ActiveSessionModel::ActiveSessionModel(Rapid::Workflow::ITrackDetectionWorkflow& trackDetector,
                                       Rapid::Workflow::IActiveSessionWorkflow& activeWorkSessionFlow,
                                       Rapid::Storage::ITrackDatabase& trackDatabase)
    : mTrackDetector{trackDetector}
    , mActiveSessionWorkFlow{activeWorkSessionFlow}
    , mTrackDatabase{trackDatabase}
{
    auto const tracks = mTrackDatabase.getTracks();
    mTrackDetector.setTracks({tracks});
    mTrackDetector.startDetection();

    mTrackDetector.trackDetected.connect([this] {
        mTrackData = mTrackDetector.getDetectedTrack();
        mTrackDetector.stopDetection();
        trackDetected.emit();
    });

    // TODO: provide these as string so we the UI doesn't to do any formatting
    mActiveSessionWorkFlow.currentLaptime.valueChanged().connect([this]() {
        currentLaptime.set(mActiveSessionWorkFlow.currentLaptime.get());
    });

    // TODO: provide these as string so we the UI doesn't to do any formatting
    mActiveSessionWorkFlow.currentSectorTime.valueChanged().connect([this]() {
        currentSectorTime.set(mActiveSessionWorkFlow.currentSectorTime.get());
    });

    mActiveSessionWorkFlow.lapCount.valueChanged().connect([this]() {
        lapCount.set(mActiveSessionWorkFlow.lapCount.get());
    });

    mActiveSessionWorkFlow.lapFinished.connect([this]() {
        lapFinished.emit();
    });
    mActiveSessionWorkFlow.sectorFinshed.connect([this]() {
        sectorFinished.emit();
    });
}

std::string ActiveSessionModel::getLastLapTime() const noexcept
{
    if (mActiveSessionWorkFlow.lastLaptime.get().asString().empty()) {
        return {"00:00:000"};
    }

    return mActiveSessionWorkFlow.lastLaptime.get().asString();
}

std::string ActiveSessionModel::getLastSector() const noexcept
{
    if (mActiveSessionWorkFlow.lastSectorTime.get().asString().empty()) {
        return {"00:00:000"};
    }

    return mActiveSessionWorkFlow.lastSectorTime.get().asString();
}

void ActiveSessionModel::confirmTrackDetection(bool confirmed)
{
    // track is not confirmed by the user so we start the track detection again.
    if (!confirmed) {
        mTrackDetector.stopDetection();
        return;
    }

    mActiveSessionWorkFlow.setTrack(mTrackDetector.getDetectedTrack());
    mActiveSessionWorkFlow.startActiveSession();
}

TrackData ActiveSessionModel::getDetectedTrack() const noexcept
{
    return mTrackData;
}
