// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <algorithm/SimpleLaptimer.hpp>
#include <algorithm/TrackDetection.hpp>
#include <positioning/IGpsPositionProvider.hpp>
#include <rest/RestServer.hpp>
#include <rest/SessionEndpoint.hpp>
#include <storage/ISessionDatabase.hpp>
#include <storage/ITrackDatabase.hpp>
#include <workflow/ActiveSessionWorkflow.hpp>
#include <workflow/TrackDetectionWorkflow.hpp>

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
