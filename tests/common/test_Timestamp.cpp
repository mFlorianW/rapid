// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include "common/Timestamp.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;

TEST_CASE("A Timestamp shall be able to convert the time into a string.")
{
    constexpr auto expectedResult = "00:01:35.123";
    Timestamp timestamp;
    timestamp.setHour(0);
    timestamp.setMinute(1);
    timestamp.setSecond(35);
    timestamp.setFractionalOfSecond(123);

    auto const result = timestamp.asString();

    REQUIRE(result == expectedResult);
}

TEST_CASE("It shall be possible to compare two timestamps for equal.")
{
    Timestamp ts1{"01:02:25.134"};
    Timestamp ts2{"01:02:25.134"};

    REQUIRE(ts1 == ts2);
}

TEST_CASE("It shall be possible to calucalte the sum of two timestamps.")
{
    Timestamp ts1{"01:02:25.134"};
    Timestamp ts2{"03:00:25.144"};
    Timestamp expectedResult{"04:02:50.278"};

    auto result = ts1 + ts2;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The plus operator of timestamp shall handle hour overflow when passing midnight.")
{
    Timestamp ts1{"22:00:00.000"};
    Timestamp ts2{"03:00:00.000"};
    Timestamp expectedResult{"01:00:00.000"};

    auto result = ts1 + ts2;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The plus operator of timestamp shall handle minute overflow when passing 60 minutes.")
{
    Timestamp ts1{"00:58:00.000"};
    Timestamp ts2{"00:12:00.000"};
    Timestamp expectedResult{"01:10:00.000"};

    auto result = ts1 + ts2;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The plus operator of timestamp shall handle second overflow when passing 60 seconds.")
{
    Timestamp ts1{"00:00:42.000"};
    Timestamp ts2{"00:00:32.000"};
    Timestamp expectedResult{"00:01:14.000"};

    auto result = ts1 + ts2;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The plus operator of timestamp shall handle millisecond overflow when passing 1000 ms.")
{
    Timestamp ts1{"00:00:00.300"};
    Timestamp ts2{"00:00:00.800"};

    Timestamp expectedResult{"00:00:01.100"};

    auto result = ts1 + ts2;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The Timestamp shall support minus operations with a other Timestamp.")
{
    Timestamp ts1{"01:02:00.134"};
    Timestamp ts2{"03:05:25.144"};
    Timestamp expectedResult{"02:03:25.010"};

    auto result = ts2 - ts1;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The Timestamp shall handle hour underflow in the minus operation.")
{
    Timestamp ts1{"05:00:00.000"};
    Timestamp ts2{"03:00:00.000"};
    Timestamp expectedResult{"22:00:00.000"};

    auto result = ts2 - ts1;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The Timestamp shall handle minute underflow in the minus operation.")
{
    Timestamp ts1{"01:48:00.000"};
    Timestamp ts2{"05:02:00.000"};
    Timestamp expectedResult{"03:14:00.000"};

    auto result = ts2 - ts1;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The Timestamp shall handle second underflow in the minus operation.")
{
    Timestamp ts1{"01:48:55.000"};
    Timestamp ts2{"05:02:55.000"};
    Timestamp expectedResult{"03:14:00.000"};

    auto result = ts2 - ts1;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The Timestamp shall handle fractional of second underflow in the minus operation.")
{
    Timestamp ts1{"01:48:55.000"};
    Timestamp ts2{"05:02:05.128"};
    Timestamp expectedResult{"03:13:10.128"};

    auto result = ts2 - ts1;

    REQUIRE(result == expectedResult);
}

TEST_CASE("The Timestamp shall be creatable from string in the format of hh:mm:ss.nnn")
{
    Timestamp expectedTs;
    expectedTs.setHour(1);
    expectedTs.setMinute(48);
    expectedTs.setSecond(55);
    expectedTs.setFractionalOfSecond(500);

    auto ts = Timestamp{"01:48:55.500"};

    REQUIRE(ts == expectedTs);
}

TEST_CASE("The Timestamp shall correctly calucalute the delta between two timestamps")
{
    Timestamp ts1{"15:05:13.237"};
    Timestamp ts2{"15:06:10.237"};

    REQUIRE((ts2 - ts1) == Timestamp{"00:00:57.000"});
}
