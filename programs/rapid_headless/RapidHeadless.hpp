// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "workflow/GpsRestResource.hpp"
#include <algorithm/SimpleLaptimer.hpp>
#include <algorithm/TrackDetection.hpp>
#include <positioning/IGPSInformationProvider.hpp>
#include <positioning/IGpsPositionProvider.hpp>
#include <rest/RestServer.hpp>
#include <rest/SessionEndpoint.hpp>
#include <storage/ISessionDatabase.hpp>
#include <storage/ITrackDatabase.hpp>
#include <workflow/ActiveSessionEndpoint.hpp>
#include <workflow/ActiveSessionWorkflow.hpp>
#include <workflow/TrackDetectionWorkflow.hpp>

namespace Rapid::LappyHeadless
{

class LappyHeadless
{
public:
    LappyHeadless(Rapid::Positioning::IGpsPositionProvider& posProvider,
                  Rapid::Positioning::IGpsInformationProvider& gpsInfoProvider,
                  Rapid::Storage::ISessionDatabase& sessionDatabase,
                  Rapid::Storage::ITrackDatabase& trackDatabase);

private:
    void hasFix(Rapid::Positioning::GpsFixMode mode);
    void startSession();

    Rapid::Positioning::IGpsPositionProvider& mPositionProvider;
    Rapid::Positioning::IGpsInformationProvider& mGpsInfoProvider;
    KDBindings::ScopedConnection mGpsFixModeConnection;
    Rapid::Storage::ISessionDatabase& mSessionDatabase;
    Rapid::Storage::ITrackDatabase& mTrackDatabase;
    Rapid::Algorithm::TrackDetection mTrackDetection{500};
    Rapid::Workflow::TrackDetectionWorkflow mTrackDetectionWorkflow{mTrackDetection, mPositionProvider};
    KDBindings::ScopedConnection mTrackDetectionConnection;
    Rapid::Algorithm::SimpleLaptimer mSimpleLaptimer{};
    Rapid::Workflow::ActiveSessionWorkflow mActiveSessionWorkflow{mPositionProvider, mSimpleLaptimer, mSessionDatabase};
    KDBindings::ScopedConnection mLapFinishedConnection;
    Rapid::Rest::SessionEndpoint mSessionEndpoint{mSessionDatabase};
    Rapid::Rest::RestServer mRestServer;
    Rapid::Workflow::ActiveSessionEndpoint mActiveSessionEndpoint{std::addressof(mActiveSessionWorkflow)};
    bool mTrackDetected{false};
    bool mHasFix{false};
    Rapid::Workflow::GpsRestResource mGpsRestResource;
};

} // namespace Rapid::LappyHeadless
