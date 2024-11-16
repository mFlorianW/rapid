// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ActiveSessionModel.hpp"
#include "ActiveSessionView.hpp"
#include "ShowMenuScreenCommand.hpp"
#include <ActiveSessionWorkflow.hpp>
#include <IGpsPositionProvider.hpp>
#include <SimpleLaptimer.hpp>
#include <TrackDetection.hpp>
#include <TrackDetectionWorkflow.hpp>

class ScreenModel;
class MainScreenModel
{
public:
    MainScreenModel(ScreenModel& screenModel,
                    Rapid::Positioning::IGpsPositionProvider& posDateTimeProvider,
                    Rapid::Storage::ISessionDatabase& sessionDatabase,
                    Rapid::Storage::ITrackDatabase& trackDatabase);
    View& getActiveView();

private:
    // Backends
    Rapid::Algorithm::TrackDetection mTrackDetector{500};
    Rapid::Algorithm::SimpleLaptimer mLapTimer;

    // ActiveSessionBackend
    Rapid::Workflow::ActiveSessionWorkflow mActiveSessionWorkflow;

    // Models
    ScreenModel& mScreenModel;
    Rapid::Workflow::TrackDetectionWorkflow mTrackDetectionWorkflow;
    ActiveSessionModel mActiveSessionModel;

    // Commands
    ShowMenuScreenCommand mShowMenuScreenCommand;

    // Views
    ActiveSessionView mActiveSessionView;
};
