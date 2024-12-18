// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <Sessions.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Correctly compare two session meta instances")
{
    SECTION("equal operator")
    {
        auto sessionMetaData0 = Rapid::TestHelper::Sessions::getTestSessionMetaData();
        auto sessionMetaData1 = Rapid::TestHelper::Sessions::getTestSessionMetaData();
        REQUIRE(sessionMetaData0 == sessionMetaData1);
    }

    SECTION("unequal operator")
    {
        auto sessionMetaData0 = Rapid::TestHelper::Sessions::getTestSessionMetaData();
        auto sessionMetaData1 = Rapid::TestHelper::Sessions::getTestSession4();
        REQUIRE(sessionMetaData0 != sessionMetaData1);
    }
}
