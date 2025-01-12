// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationConfig.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::RapidShell;

SCENARIO("The application version shall be constructable from a string")
{
    auto const expectedVersion = ApplicationVersion{1, 2, 3};
    auto const stringVersion = QString{"1.2.3"};
    WHEN("The application version is created from the string")
    {
        auto const version = ApplicationVersion::fromString(stringVersion);
        THEN("The correct application shall be created.")
        {
            REQUIRE(version == expectedVersion);
        }
    }
}
