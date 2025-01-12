// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <common/VelocityData.hpp>

using namespace Rapid::Common;

SCENARIO("The velocity shall give the velocity in m/s")
{
    GIVEN("Nothing")
    {
        WHEN("Creating a velocity instance with km/h")
        {
            auto velocity = VelocityData::createFromKmH(36.0);
            THEN("The correct VelocityData is created")
            {
                REQUIRE(velocity.getVelocity() == 10.0);
            }
        }

        WHEN("Creating a velocity instance with mp/h")
        {
            auto velocity = VelocityData::createFromMpH(10);
            THEN("The correct VelocityData is created")
            {
                REQUIRE(velocity.getVelocity() == 4.4704);
            }
        }
    }
}

SCENARIO("The velocity shall be comparable")
{
    GIVEN("Some velocities")
    {
        auto const vel1 = VelocityData{10.0};
        auto const vel2 = VelocityData{10.0};
        auto const vel3 = VelocityData{20.0};
        WHEN("Comparing the same velocities with equal operator")
        {
            auto const same = vel1 == vel2;
            THEN("The result of the comparsion is true")
            {
                REQUIRE(same == true);
            }
        }

        WHEN("Comparing not the same velocities with equal operator")
        {
            auto const same = vel1 == vel3;
            THEN("The result of the comparsion is false")
            {
                REQUIRE(same == false);
            }
        }

        WHEN("Comparing the same velocities with unequal operator")
        {
            auto const same = vel1 != vel2;
            THEN("The result of the comparsion is false")
            {
                REQUIRE(same == false);
            }
        }

        WHEN("Comparing not the same velocities with unequal operator")
        {
            auto const same = vel1 != vel3;
            THEN("The result of the comparsion is false")
            {
                REQUIRE(same == true);
            }
        }
    }
}
