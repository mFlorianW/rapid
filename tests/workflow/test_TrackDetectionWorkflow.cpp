// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "algorithm/TrackDetection.hpp"
#include "testhelper/PositionDateTimeProvider.hpp"
#include "testhelper/Positions.hpp"
#include "testhelper/Tracks.hpp"
#include "workflow/TrackDetectionWorkflow.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Workflow;
using namespace Rapid::Algorithm;
using namespace Rapid::TestHelper;

TEST_CASE("TrackDetectionWorkflow shall emit 'trackDetected' when successful detected a track.")
{
    auto trackDetector = TrackDetection{500};
    auto posInfoProvider = PositionDateTimeProvider{};
    auto tdw = TrackDetectionWorkflow{trackDetector, posInfoProvider};
    bool trackDetectedEmitted{false};

    std::ignore = tdw.trackDetected.connect([&] {
        trackDetectedEmitted = true;
    });
    tdw.setTracks({Tracks::getOscherslebenTrack()});
    tdw.startDetection();

    posInfoProvider.gpsPosition.set({{Positions::getOscherslebenPositionCamp()}, {}, {}});

    REQUIRE(trackDetectedEmitted == true);
}

TEST_CASE("TrackDetectionWorkflow shall return the TrackData when successful detected a track.")
{
    auto trackDetector = TrackDetection{500};
    auto posInfoProvider = PositionDateTimeProvider{};
    auto tdw = TrackDetectionWorkflow{trackDetector, posInfoProvider};

    tdw.setTracks({Tracks::getOscherslebenTrack()});
    tdw.startDetection();

    posInfoProvider.gpsPosition.set({{Positions::getOscherslebenPositionCamp()}, {}, {}});

    REQUIRE(tdw.getDetectedTrack() == Tracks::getOscherslebenTrack());
}

TEST_CASE("TrackDetectionWorkflow shall not emit 'trackDetected' when stopped.")
{
    auto trackDetector = TrackDetection{500};
    auto posInfoProvider = PositionDateTimeProvider{};
    auto tdw = TrackDetectionWorkflow{trackDetector, posInfoProvider};
    bool trackDetectedEmitted{false};

    std::ignore = tdw.trackDetected.connect([&] {
        trackDetectedEmitted = true;
    });
    tdw.setTracks({Tracks::getOscherslebenTrack()});

    tdw.startDetection();
    posInfoProvider.gpsPosition.set({{Positions::getOscherslebenPositionCamp()}, {}, {}});

    REQUIRE(trackDetectedEmitted == true);

    trackDetectedEmitted = false;
    tdw.stopDetection();
    posInfoProvider.gpsPosition.set({{Positions::getOscherslebenPositionCamp()}, {}, {}});

    REQUIRE(trackDetectedEmitted == false);
}
