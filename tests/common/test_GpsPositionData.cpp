// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include "common/GpsPositionData.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;

TEST_CASE("It shall be possible to compare two PositionDateTime for equal")
{
    PositionData pos;
    pos.setLatitude(22);
    pos.setLatitude(23);

    Timestamp ts;
    ts.setHour(0);
    ts.setMinute(0);
    ts.setSecond(0);

    Date date;
    date.setYear(1970);
    date.setDay(1);
    date.setMonth(1);

    auto velocity = VelocityData{10};

    GpsPositionData posDateTime1;
    posDateTime1.setPosition(pos);
    posDateTime1.setDate(date);
    posDateTime1.setTime(ts);
    posDateTime1.setVelocity(velocity);
    GpsPositionData posDateTime2;
    posDateTime2.setPosition(pos);
    posDateTime2.setDate(date);
    posDateTime2.setTime(ts);
    posDateTime2.setVelocity(velocity);

    REQUIRE(posDateTime1 == posDateTime2);
}

TEST_CASE("It shall be possible to compare two PositionDateTime for unequal")
{
    PositionData pos;
    pos.setLatitude(22);
    pos.setLatitude(23);

    PositionData pos2;
    pos.setLatitude(22);
    pos.setLatitude(24);

    Timestamp ts;
    ts.setHour(0);
    ts.setMinute(0);
    ts.setSecond(0);

    Date date;
    date.setYear(1970);
    date.setDay(1);
    date.setMonth(1);

    GpsPositionData posDateTime1;
    posDateTime1.setPosition(pos);
    posDateTime1.setDate(date);
    posDateTime1.setTime(ts);
    posDateTime1.setVelocity(VelocityData{10});
    GpsPositionData posDateTime2;
    posDateTime2.setPosition(pos2);
    posDateTime2.setDate(date);
    posDateTime2.setTime(ts);

    REQUIRE(posDateTime1 != posDateTime2);
}
