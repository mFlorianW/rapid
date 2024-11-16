// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsEndpoint.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Rest;
using namespace Rapid::Common;

namespace
{
auto const request = std::string{"{\"date\":\"01.00.0123\","
                                 "\"latitude\":\"52.02739715576172\","
                                 "\"longitude\":\"11.278840065002441\","
                                 "\"time\":\"17:19:05.045\"}"};

} // namespace

TEST_CASE("The GpsEndpoint shall parse the JSON RestRequest and shall update the PositionDateTime Property")
{
    auto restRequest = RestRequest{RequestType::Post, "/gps", request};
    auto source = GpsEndpoint{};
    auto expectedResult = PositionDateTimeData{};
    expectedResult.setDate({"01.00.0123"});
    expectedResult.setTime({"17:19:05.045"});
    expectedResult.setPosition({52.02739715576172, 11.278840065002441});

    auto const handleResult = source.handleRestRequest(restRequest);

    REQUIRE(handleResult == RequestHandleResult::Ok);
    REQUIRE(source.positionTimeData.get() == expectedResult);
}
