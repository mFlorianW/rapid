// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <testhelper/GpsInformationProvider.hpp>
#include <testhelper/GpsPositions.hpp>
#include <testhelper/PositionDateTimeProvider.hpp>
#include <testhelper/SignalSpy.hpp>
#include <workflow/GpsRestResource.hpp>

using namespace Rapid::TestHelper;
using namespace Rapid::Positioning;
using namespace Rapid::Rest;
using namespace Rapid::Common;

namespace
{
struct TestFixture
{
    GpsPositionInformationProvider gpsInfoProvider;
    PositionDateTimeProvider gpsPosProvider;
    Rapid::Workflow::GpsRestResource gpsRestResource{&gpsInfoProvider, &gpsPosProvider};
    SignalSpy<RequestHandleResult, RestRequest> finishedSpy = SignalSpy{gpsRestResource.finished};
};

} // namespace

TEST_CASE_METHOD(TestFixture,
                 "The GpsRestResource shall provide the fix mode of the GPS modul",
                 "[GPS_REST_RESOURCE][FIX_MODE]")
{

    SECTION("Provide the GPS status NoFix on \"/gps/fixmode\"")
    {
        REQUIRE_CALL(gpsInfoProvider, getGpsFixMode()).RETURN(GpsFixMode::NoFix);
        auto request = RestRequest{RequestType::Get, "/gps/fixmode"};

        gpsRestResource.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Ok);
        CHECK(response.getReturnType() == RequestReturnType::Json);
        REQUIRE(response.getReturnBody() == "{\"fixmode\":\"nofix\"}");
    }

    SECTION("Provide the GPS status Fix2d on \"/gps/fixmode\"")
    {
        REQUIRE_CALL(gpsInfoProvider, getGpsFixMode()).RETURN(GpsFixMode::Fix2d);
        auto request = RestRequest{RequestType::Get, "/gps/fixmode"};

        gpsRestResource.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Ok);
        CHECK(response.getReturnType() == RequestReturnType::Json);
        REQUIRE(response.getReturnBody() == "{\"fixmode\":\"fix2d\"}");
    }

    SECTION("Provide the GPS status Fix3d on \"/gps/fixmode\"")
    {
        REQUIRE_CALL(gpsInfoProvider, getGpsFixMode()).RETURN(GpsFixMode::Fix3d);
        auto request = RestRequest{RequestType::Get, "/gps/fixmode"};

        gpsRestResource.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Ok);
        CHECK(response.getReturnType() == RequestReturnType::Json);
        REQUIRE(response.getReturnBody() == "{\"fixmode\":\"fix3d\"}");
    }
}

TEST_CASE_METHOD(TestFixture,
                 "Provide the number of used satellites on \"/gps/satellites\"",
                 "[GPS_REST_RESOURCE][SATELLITES]")
{
    REQUIRE_CALL(gpsInfoProvider, getNumbersOfStatelite()).RETURN(12);
    auto request = RestRequest{RequestType::Get, "/gps/satellites"};

    gpsRestResource.handleRestRequest(request);

    REQUIRE(finishedSpy.getCount() == 1);
    auto [result, response] = finishedSpy.at(0);
    CHECK(result == RequestHandleResult::Ok);
    CHECK(response.getReturnType() == RequestReturnType::Json);
    REQUIRE(response.getReturnBody() == "{\"satellites\":12}");
}

TEST_CASE_METHOD(TestFixture, "Provide the GPS position on \"/gps/position\"", "[GPS_REST_RESOURCE][POSITION]")
{
    gpsPosProvider.gpsPosition.set(GpsPositions::getGpsPosition());
    auto request = RestRequest{RequestType::Get, "/gps/position"};

    gpsRestResource.handleRestRequest(request);

    REQUIRE(finishedSpy.getCount() == 1);
    auto [result, response] = finishedSpy.at(0);
    CHECK(result == RequestHandleResult::Ok);
    CHECK(response.getReturnType() == RequestReturnType::Json);
    REQUIRE(response.getReturnBody() == GpsPositions::getGpsPositionAsJson());
}

TEST_CASE_METHOD(TestFixture, "Handle different errors correctly", "[GPS_REST_RESOURCE][GENERAL_ERROR]")
{
    SECTION("Invalid sub Path")
    {
        gpsPosProvider.gpsPosition.set(GpsPositions::getGpsPosition());
        auto request = RestRequest{RequestType::Get, "/gps/sdfsdfsdfsd"};

        gpsRestResource.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Error);
    }

    SECTION("Missing sub path")
    {
        gpsPosProvider.gpsPosition.set(GpsPositions::getGpsPosition());
        auto request = RestRequest{RequestType::Get, "/gps"};

        gpsRestResource.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Error);
    }

    SECTION("Wrong root")
    {
        gpsPosProvider.gpsPosition.set(GpsPositions::getGpsPosition());
        auto request = RestRequest{RequestType::Get, "/aaaa"};

        gpsRestResource.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Error);
    }

    SECTION("Empty path")
    {
        gpsPosProvider.gpsPosition.set(GpsPositions::getGpsPosition());
        auto request = RestRequest{RequestType::Get, ""};

        gpsRestResource.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Error);
    }
}
