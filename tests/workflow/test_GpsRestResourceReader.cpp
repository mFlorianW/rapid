// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <testhelper/RestClientMock.hpp>
#include <testhelper/SignalSpy.hpp>
#include <workflow/GpsRestResourceReader.hpp>

using namespace Rapid::Workflow;
using namespace Rapid::TestHelper;
using namespace Rapid::Rest;
using namespace Rapid::Positioning;
using namespace Rapid::System;
using namespace trompeloeil;

namespace
{

struct TestFixture
{
    RestClientMock restClient;
    GpsRestResourceReader gpsRestResourceReader{&restClient};
    std::shared_ptr<RestCallMock> restCall = std::make_shared<RestCallMock>();
};

} // namespace

TEST_CASE_METHOD(TestFixture,
                 "The GpsRestResourceReader shall fetch the GPS fix mode",
                 "[GPS_RESOURCE_READER][FIX_MODE]")
{
    SECTION("nofix")
    {
        REQUIRE_CALL(restClient, execute(_))
            .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path("/gps/fixmode"))
            .LR_SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
            .LR_RETURN(restCall);

        auto const result = std::string{R"({"fixmode":"nofix"})"};
        restCall->setData(result);
        auto fixModeResult = gpsRestResourceReader.getFixMode();
        REQUIRE(fixModeResult->getResult() == Result::Ok);
        REQUIRE(fixModeResult->getResultValue().value_or(GpsFixMode::Fix3d) == GpsFixMode::NoFix);
    }

    SECTION("fix2d")
    {
        REQUIRE_CALL(restClient, execute(_))
            .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path("/gps/fixmode"))
            .LR_SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
            .LR_RETURN(restCall);

        auto const result = std::string{R"({"fixmode":"fix2d"})"};
        restCall->setData(result);
        auto fixModeResult = gpsRestResourceReader.getFixMode();
        REQUIRE(fixModeResult->getResult() == Result::Ok);
        REQUIRE(fixModeResult->getResultValue().value_or(GpsFixMode::Fix3d) == GpsFixMode::Fix2d);
    }

    SECTION("fix3d")
    {
        REQUIRE_CALL(restClient, execute(_))
            .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path("/gps/fixmode"))
            .LR_SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
            .LR_RETURN(restCall);

        auto const result = std::string{R"({"fixmode":"fix3d"})"};
        restCall->setData(result);
        auto fixModeResult = gpsRestResourceReader.getFixMode();
        REQUIRE(fixModeResult->getResult() == Result::Ok);
        REQUIRE(fixModeResult->getResultValue().value_or(GpsFixMode::Fix2d) == GpsFixMode::Fix3d);
    }
}

TEST_CASE_METHOD(TestFixture,
                 "The GpsRestResourceReader shall fetch the GPS fix mode",
                 "[GPS_RESOURCE_READER][SATELLITES]")
{
    REQUIRE_CALL(restClient, execute(_))
        .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path("/gps/satellites"))
        .LR_SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
        .LR_RETURN(restCall);

    auto const result = std::string{R"({"satellites":12})"};
    restCall->setData(result);
    auto satellitesResult = gpsRestResourceReader.getSatellites();
    REQUIRE(satellitesResult->getResult() == Result::Ok);
    REQUIRE(satellitesResult->getResultValue().value_or(0) == 12);
}
