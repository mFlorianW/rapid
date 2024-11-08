// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MainScreenModel.hpp"

// Setup ActiveSessionView

MainScreenModel::MainScreenModel(ScreenModel& screenModel,
                                 Rapid::Positioning::IPositionDateTimeProvider& posDateTimeProvider,
                                 Rapid::Storage::ISessionDatabase& sessionDatabase,
                                 Rapid::Storage::ITrackDatabase& trackDatabase)
    : mActiveSessionWorkflow{posDateTimeProvider, mLapTimer, sessionDatabase}
    , mScreenModel{screenModel}
    , mTrackDetectionWorkflow{mTrackDetector, posDateTimeProvider}
    , mActiveSessionModel{mTrackDetectionWorkflow, mActiveSessionWorkflow, trackDatabase}
    , mShowMenuScreenCommand{mScreenModel}
    , mActiveSessionView{mActiveSessionModel}
{
    mActiveSessionView.setShowMenuScreenCommand(&mShowMenuScreenCommand);
}

View& MainScreenModel::getActiveView()
{
    return mActiveSessionView;
}
