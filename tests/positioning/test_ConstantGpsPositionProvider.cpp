// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "positioning/ConstantGpsPositionProvider.hpp"
#include "testhelper/CompareHelper.hpp"
#include <catch2/catch_all.hpp>
#include <common/PositionData.hpp>

using namespace Rapid::Positioning;
using namespace Rapid::Common;
using namespace Rapid::System;
using namespace Rapid::Testhelper;
using namespace Catch;

constexpr auto timeout = std::chrono::seconds{1};

TEST_CASE("The ConstantGpsPositionProvider shall interpolate the position between two points if the given "
          "point data is insufficent.")
{
    auto positions = std::vector<PositionData>{{52.026649, 11.282535}, {52.026751, 11.282047}, {52.026807, 11.281746}};
    auto lastPosition = GpsPositionData{};

    auto source = ConstantGpsPositionProvider{positions};
    std::ignore = source.gpsPosition.valueChanged().connect([&]() {
        lastPosition = source.gpsPosition.get();
    });
    source.setVelocityInMeterPerSecond(2.77778);
    source.start();

    auto expectedPosition = GpsPositionData{{52.026649, 11.282535}, {}, {}};
    REQUIRE_COMPARE_WITH_TIMEOUT(lastPosition.getPosition().getLatitude(),
                                 expectedPosition.getPosition().getLatitude(),
                                 timeout);
    REQUIRE(lastPosition.getPosition().getLongitude() == expectedPosition.getPosition().getLongitude());

    expectedPosition = GpsPositionData{{52.02665, 11.28253}, {}, {}};
    REQUIRE_COMPARE_WITH_TIMEOUT(lastPosition.getPosition().getLatitude(),
                                 expectedPosition.getPosition().getLatitude(),
                                 timeout);
    REQUIRE(lastPosition.getPosition().getLongitude() == Approx(expectedPosition.getPosition().getLongitude()));
}

TEST_CASE("The ConstantGpsPositionProvider shall provide every 100ms a new position.")
{
    std::uint8_t updateCounter = 0;
    auto positions = std::vector<PositionData>{{52.026649, 11.282535}, {52.026751, 11.282047}, {52.026807, 11.281746}};
    auto source = ConstantGpsPositionProvider{positions};
    source.setVelocityInMeterPerSecond(2.77778);
    std::ignore = source.gpsPosition.valueChanged().connect([&]() {
        ++updateCounter;
    });
    // update postion when source is started
    source.start();
    REQUIRE_COMPARE_WITH_TIMEOUT(updateCounter, 1, timeout);
    REQUIRE_COMPARE_WITH_TIMEOUT(updateCounter, 2, timeout);

    // no updates when source is stopped
    source.stop();
    REQUIRE_COMPARE_WITH_TIMEOUT(updateCounter, 2, timeout);
}

TEST_CASE("The ConstantGpsPositionProvider shall provide the velocity in the reported GpsPositionData.")
{
    auto positions = std::vector<PositionData>{{52.026649, 11.282535}, {52.026751, 11.282047}, {52.026807, 11.281746}};
    auto lastPosition = GpsPositionData{};

    auto source = ConstantGpsPositionProvider{positions};
    std::ignore = source.gpsPosition.valueChanged().connect([&]() {
        lastPosition = source.gpsPosition.get();
    });
    source.setVelocityInMeterPerSecond(10);
    source.start();

    REQUIRE_COMPARE_WITH_TIMEOUT(lastPosition.getVelocity().getVelocity(), 10, timeout);
}
