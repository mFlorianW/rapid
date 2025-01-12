// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "system/Event.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::System;

SCENARIO("The Event shall give the type")
{
    GIVEN("A Event")
    {
        auto const event = Event{};
        WHEN("Requesting the type")
        {
            auto const eventType = event.getEventType();
            THEN("The correct type shall be returned")
            {
                REQUIRE(eventType == Event::Type::Unknown);
            }
        }
    }
}
