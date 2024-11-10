// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "SessionEndpoint.hpp"
#include <ActiveSessionWorkflow.hpp>
#include <IGpsPositionProvider.hpp>
#include <ISessionDatabase.hpp>
#include <ITrackDatabase.hpp>
#include <RestServer.hpp>
#include <SimpleLaptimer.hpp>
#include <TrackDetection.hpp>
#include <TrackDetectionWorkflow.hpp>

namespace Rapid::LappyHeadless
{

class LappyHeadless
{
public:
    LappyHeadless(Rapid::Positioning::IGpsPositionProvider& posProvider,
                  Rapid::Storage::ISessionDatabase& sessionDatabase,
                  Rapid::Storage::ITrackDatabase& trackDatabase);

private:
    Rapid::Positioning::IGpsPositionProvider& mPositionProvider;
    Rapid::Storage::ISessionDatabase& mSessionDatabase;
    Rapid::Storage::ITrackDatabase& mTrackDatabase;
    Rapid::Algorithm::TrackDetection mTrackDetection{500};
    Rapid::Workflow::TrackDetectionWorkflow mTrackDetectionWorkflow{mTrackDetection, mPositionProvider};
    Rapid::Algorithm::SimpleLaptimer mSimpleLaptimer{};
    Rapid::Workflow::ActiveSessionWorkflow mActiveSessionWorkflow{mPositionProvider, mSimpleLaptimer, mSessionDatabase};
    Rapid::Rest::SessionEndpoint mSessionEndpoint{mSessionDatabase};
    Rapid::Rest::RestServer mRestServer;
};

} // namespace Rapid::LappyHeadless
