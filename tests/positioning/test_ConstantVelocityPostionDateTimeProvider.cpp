// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "CompareHelper.hpp"
#include "ConstantVelocityPositionDateTimeProvider.hpp"
#include <PositionData.hpp>
#include <catch2/catch_all.hpp>

using namespace Rapid::Positioning;
using namespace Rapid::Common;
using namespace Rapid::System;
using namespace Rapid::Testhelper;
using namespace Catch;

constexpr auto timeout = std::chrono::seconds{1};

TEST_CASE("The ConstantVelocityPositionDateTimeProvider shall interpolate the position between two points if the given "
          "point data is insufficent.")
{
    auto positions = std::vector<PositionData>{{52.026649, 11.282535}, {52.026751, 11.282047}, {52.026807, 11.281746}};
    auto lastPosition = PositionDateTimeData{};

    auto source = ConstantVelocityPositionDateTimeProvider{positions};
    source.positionTimeData.valueChanged().connect([&]() {
        lastPosition = source.positionTimeData.get();
    });
    source.setVelocityInMeterPerSecond(2.77778);
    source.start();

    auto expectedPosition = PositionDateTimeData{{52.026649, 11.282535}, {}, {}};
    REQUIRE_COMPARE_WITH_TIMEOUT(lastPosition.getPosition().getLatitude(),
                                 expectedPosition.getPosition().getLatitude(),
                                 timeout);
    REQUIRE(lastPosition.getPosition().getLongitude() == expectedPosition.getPosition().getLongitude());

    expectedPosition = PositionDateTimeData{{52.02665, 11.28253}, {}, {}};
    REQUIRE_COMPARE_WITH_TIMEOUT(lastPosition.getPosition().getLatitude(),
                                 expectedPosition.getPosition().getLatitude(),
                                 timeout);
    REQUIRE(lastPosition.getPosition().getLongitude() == Approx(expectedPosition.getPosition().getLongitude()));
}

TEST_CASE("The ConstantVelocityPositionDateTimeProvider shall provide every 100ms a new position.")
{
    std::uint8_t updateCounter = 0;
    auto positions = std::vector<PositionData>{{52.026649, 11.282535}, {52.026751, 11.282047}, {52.026807, 11.281746}};
    auto source = ConstantVelocityPositionDateTimeProvider{positions};
    source.setVelocityInMeterPerSecond(2.77778);
    source.positionTimeData.valueChanged().connect([&]() {
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
