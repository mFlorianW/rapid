// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <rest/RestRequest.hpp>
#include <testhelper/ActiveSessionMock.hpp>
#include <testhelper/SignalSpy.hpp>
#include <testhelper/Tracks.hpp>
#include <workflow/ActiveSessionEndpoint.hpp>
#include <workflow/IActiveSessionWorkflow.hpp>

using namespace Rapid::Workflow;
using namespace Rapid::Rest;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;
using namespace trompeloeil;

namespace
{

class TestFixture
{
public:
    ActiveSessionMock activeSessionMock;
    ActiveSessionEndpoint endpoint{std::addressof(activeSessionMock)};
    SignalSpy<RequestHandleResult, RestRequest> finishedSpy = SignalSpy{endpoint.finished};
};

} // namespace

TEST_CASE_METHOD(TestFixture,
                 "The ActiveSessionEndpoint shall provide the active session data via REST",
                 "[ACTIVE_SESSION_ENDPOINT]")
{
    SECTION("Handle invalid root path")
    {
        auto request = RestRequest{RequestType::Get, "/asdf"};

        endpoint.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Error);
    }

    SECTION("Handle invalid sub path")
    {
        auto request = RestRequest{RequestType::Get, "/activeSession/asdf"};

        endpoint.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Error);
    }

    SECTION("Provide the track on \"/activeSession/track\" path")
    {
        auto request = RestRequest{RequestType::Get, "/activeSession/track"};
        REQUIRE_CALL(activeSessionMock, getTrack()).LR_RETURN(Tracks::getTrack());

        endpoint.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Ok);
        CHECK(response.getReturnType() == RequestReturnType::Json);
        REQUIRE(response.getReturnBody() == Tracks::getTrackAsJson());
    }

    SECTION("Provide the current lap information \"/activeSession/lap\"")
    {
        auto request = RestRequest{RequestType::Get, "/activeSession/lap"};
        // clang-format off
        auto expReturnBody = std::string
        {
        "{"
                "\"lapCount\":10,"
                "\"currentLap\":\"00:01:25.123\","
                "\"currentSector\":\"00:00:25.123\","
                "\"lastLap\":\"00:01:25.123\","
                "\"lastSector\":\"00:00:25.123\""
           "}"
        };
        // clang-format on
        activeSessionMock.lapCount.set(10);
        activeSessionMock.currentLaptime.set(Timestamp{"00:01:25.123"});
        activeSessionMock.currentSectorTime.set(Timestamp{"00:00:25.123"});
        activeSessionMock.lastLaptime.set(Timestamp{"00:01:25.123"});
        activeSessionMock.lastSectorTime.set(Timestamp{"00:00:25.123"});

        endpoint.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, response] = finishedSpy.at(0);
        CHECK(result == RequestHandleResult::Ok);
        CHECK(response.getReturnType() == RequestReturnType::Json);
        REQUIRE(response.getReturnBody() == expReturnBody);
    }
}
