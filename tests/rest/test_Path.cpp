// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rest/Path.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Rest;

SCENARIO("A path shall give the number of level in the path")
{
    GIVEN("A path with three levels")
    {
        auto const path = Path{"/test/path/dup"};
        constexpr auto expectedLevels = 3;

        WHEN("The path levels are requested.")
        {
            auto const levels = path.getDepth();
            THEN("The correct level depth shall be returned")
            {
                REQUIRE(levels == expectedLevels);
            }
        }
    }

    GIVEN("A empty path")
    {
        auto const path = Path{""};
        constexpr auto expectedLevels = 0;

        WHEN("The path level is requested")
        {
            auto const levels = path.getDepth();
            THEN("The correct level of the path shall be returned")
            {
                REQUIRE(levels == expectedLevels);
            }
        }
    }
}

SCENARIO("A path shall give the path elements addressed by the element index")
{
    GIVEN("A path with three levels")
    {
        auto const path = Path{"/test/path/dup"};

        WHEN("The path element with index 0 is requested")
        {
            auto const element = path.getEntry(0);
            auto const expectedElement = "test";
            THEN("The correct entry shall be returned")
            {
                REQUIRE(element.has_value() == true);
                REQUIRE(element.value_or("") == expectedElement);
            }
        }

        WHEN("The path element with index 1 is requested")
        {
            auto const element = path.getEntry(1);
            auto const expectedElement = "path";
            THEN("The correct entry shall be returned")
            {
                REQUIRE(element.has_value() == true);
                REQUIRE(element.value_or("") == expectedElement);
            }
        }

        WHEN("The path element with index 2 is requested")
        {
            auto const element = path.getEntry(2);
            auto const expectedElement = "dup";
            THEN("The correct entry shall be returned")
            {
                REQUIRE(element.has_value() == true);
                REQUIRE(element.value_or("") == expectedElement);
            }
        }

        WHEN("A path element with an invalid index is requested")
        {
            auto const element = path.getEntry(100);
            THEN("A nullopt shall be returned")
            {
                REQUIRE(element.has_value() == false);
            }
        }
    }

    GIVEN("A empty Path")
    {
        auto const path = Path{""};
        WHEN("The path element of an empty path is requested")
        {
            auto const element = path.getEntry(2);
            THEN("A nullopt shall be returned")
            {
                REQUIRE(element.has_value() == false);
            }
        }
    }

    GIVEN("A path with on level")
    {
        auto const path = Path{"/sessions"};
        WHEN("The path element for index 0 is requested")
        {
            auto const element = path.getEntry(0);
            auto const expectedElement = "sessions";
            THEN("The correct entry shall be returned")
            {
                REQUIRE(element.has_value() == true);
                REQUIRE(element.value_or("") == expectedElement);
            }
        }
    }
}

SCENARIO("The Path shall return the whole path as string")
{
    GIVEN("A correctly initialized path")
    {
        constexpr auto rawPath = "/sessions2";
        auto const path = Path{rawPath};

        WHEN("The complete path as string is requested")
        {
            auto const returnPath = path.getPath();
            THEN("The correct path will be returned")
            {
                REQUIRE(returnPath == rawPath);
            }
        }
    }
}
