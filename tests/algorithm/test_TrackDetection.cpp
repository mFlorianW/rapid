// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "algorithm/TrackDetection.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;
using namespace Rapid::Algorithm;

TEST_CASE("Return true when position is on the track.")
{
    auto trackDetection = TrackDetection{500};
    auto oscherslebenTrack = TrackData{};
    auto finishLine = PositionData{};
    finishLine.setLatitude(52.02718520);
    finishLine.setLongitude(11.27989104);
    oscherslebenTrack.setFinishline(finishLine);

    auto positionTest = PositionData{};
    positionTest.setLatitude(52.0258333);
    positionTest.setLongitude(11.279166666);

    REQUIRE(trackDetection.isOnTrack(oscherslebenTrack, positionTest) == true);
}

TEST_CASE("Return false when position is not on the track.")
{
    auto trackDetection = TrackDetection{500};
    auto oscherslebenTrack = TrackData{};
    auto finishLine = PositionData{};
    finishLine.setLatitude(52.02718520);
    finishLine.setLongitude(11.27989104);
    oscherslebenTrack.setFinishline(finishLine);

    auto invalidPositionData = PositionData{};
    invalidPositionData.setLatitude(52.0225);
    invalidPositionData.setLongitude(11.277777777777779);

    REQUIRE(trackDetection.isOnTrack(oscherslebenTrack, invalidPositionData) == false);
}
