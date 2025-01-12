// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include "common/LapData.hpp"
#include "common/Timestamp.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;

TEST_CASE("The LapData shall calculate the laptime by the sector times.")
{
    Timestamp sectorTime1;
    sectorTime1.setHour(0);
    sectorTime1.setMinute(0);
    sectorTime1.setSecond(25);
    sectorTime1.setFractionalOfSecond(134);
    Timestamp sectorTime2;
    sectorTime2.setHour(0);
    sectorTime2.setMinute(0);
    sectorTime2.setSecond(25);
    sectorTime2.setFractionalOfSecond(144);

    LapData lap;
    lap.addSectorTime(sectorTime1);
    lap.addSectorTime(sectorTime2);

    REQUIRE(lap.getLaptime().getHour() == 0);
    REQUIRE(lap.getLaptime().getMinute() == 0);
    REQUIRE(lap.getLaptime().getSecond() == 50);
    REQUIRE(lap.getLaptime().getFractionalOfSecond() == 278);
}

TEST_CASE("Create LapData from vector of sector times")
{
    auto sectorTime = Timestamp{"00:00:45.112"};

    auto lap = LapData{std::vector<Timestamp>{sectorTime}};

    REQUIRE(lap.getLaptime() == Timestamp{"00:00:45.112"});
    REQUIRE(lap.getSectorTimeCount() == 1);
    REQUIRE(lap.getSectorTime(0).has_value() == true);
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(lap.getSectorTime(0).value() == Timestamp{"00:00:45.112"});
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("Create LapData from a timestamp that is used as laptime for the case the track doesn't have any sectors")
{
    auto laptime = Timestamp{"00:00:45.112"};

    auto lap = LapData{laptime};

    REQUIRE(lap.getLaptime() == Timestamp{"00:00:45.112"});
}

TEST_CASE("The LapData shall store the position and time data for that lap.")
{
    auto lap = LapData{};
    auto const expPos1 = GpsPositionData{PositionData{0.12, 0.13}, Timestamp{"00:12:33.123"}, Date{"01.01.1970"}};
    auto const expPos2 = GpsPositionData{PositionData{0.12, 0.13}, Timestamp{"00:12:33.124"}, Date{"01.01.1970"}};

    SECTION("Add single position")
    {
        lap.addPosition(expPos1);
        auto const& positions = lap.getPositions();
        REQUIRE(positions.size() == 1);
        REQUIRE(positions.at(0) == expPos1);
    }

    SECTION("Overwrite all positions")
    {
        lap.overwritePositions({expPos1, expPos2});
        auto const& positions = lap.getPositions();
        REQUIRE(positions.size() == 2);
        REQUIRE(positions == std::vector<GpsPositionData>{expPos1, expPos2});
    }
}
