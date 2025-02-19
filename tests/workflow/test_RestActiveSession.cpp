// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "common/Timestamp.hpp"
#include "testhelper/SignalSpy.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <common/TrackData.hpp>
#include <testhelper/RestClientMock.hpp>
#include <testhelper/Tracks.hpp>
#include <workflow/RestActiveSession.hpp>

using namespace trompeloeil;
using namespace Rapid::Workflow;
using namespace Rapid::TestHelper;
using namespace Rapid::Rest;
using namespace Rapid::Common;

namespace
{
struct TestFixture
{
    RestClientMock restClient;
};

} // namespace

TEST_CASE_METHOD(TestFixture,
                 "The ActiveSessionEndpointClientshall read track"
                 "ACTIVE_SESSION_ENDPOINT_CLIENT_TRACK")
{
    auto expectedReq = RestRequest{RequestType::Get, "/activeSession/track"};
    auto restCall = std::make_shared<RestCallMock>();
    REQUIRE_CALL(restClient, execute(_)).LR_WITH(_1 == expectedReq).LR_RETURN(restCall);
    auto restActiveSession = RestActiveSession{std::addressof(restClient)};

    SECTION("Send out correct HTTP REQUEST")
    {
        restActiveSession.updateTrackData();
    }

    SECTION("Success download shall update the track property")
    {
        auto updateSpy = SignalSpy{restActiveSession.track.valueChanged()};
        restActiveSession.updateTrackData();
        restCall->setData(Tracks::getTrackAsJson());
        restCall->setCallResult(RestCallResult::Success);
        REQUIRE(updateSpy.getCount() == 1);
        REQUIRE(restActiveSession.track.get() == Tracks::getTrack());
    }

    SECTION("Handle failing track udpate call")
    {
        auto updateSpy = SignalSpy{restActiveSession.track.valueChanged()};
        restActiveSession.updateTrackData();
        restCall->setCallResult(RestCallResult::Error);
        REQUIRE(updateSpy.getCount() == 0);
        REQUIRE(restActiveSession.track.get() == TrackData{});
    }
}

TEST_CASE_METHOD(TestFixture, "The RestActiveSession shall update the current lap time")
{
    auto expectedReq = RestRequest{RequestType::Get, "/activeSession/lap"};
    auto restCall = std::make_shared<RestCallMock>();
    REQUIRE_CALL(restClient, execute(_)).LR_WITH(_1 == expectedReq).LR_RETURN(restCall);
    auto restActiveSession = RestActiveSession{std::addressof(restClient)};

    SECTION("Sends correct HTTP request")
    {
        restActiveSession.updateLapData();
    }

    SECTION("Updates the lap properties correctly")
    {
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
        restActiveSession.updateLapData();
        restCall->setData(expReturnBody);
        restCall->setCallResult(RestCallResult::Success);
        REQUIRE(restActiveSession.lapCount.get() == 10);
        REQUIRE(restActiveSession.currentLap.get() == Timestamp{"00:01:25.123"});
        REQUIRE(restActiveSession.currentSector.get() == Timestamp{"00:00:25.123"});
        REQUIRE(restActiveSession.lastLap.get() == Timestamp{"00:01:25.123"});
        REQUIRE(restActiveSession.lastSector.get() == Timestamp{"00:00:25.123"});
    }
}
