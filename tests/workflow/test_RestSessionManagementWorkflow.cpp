// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "testhelper/Sessions.hpp"
#include "workflow/RestSessionManagementWorkflow.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <testhelper/RestClientMock.hpp>
#include <testhelper/SignalSpy.hpp>

using namespace Rapid::Workflow;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;
using namespace Rapid::Rest;
using namespace trompeloeil;

constexpr auto SessionCountJson = R"({"count":2})";

SCENARIO("The RestSessionManagementWorkflow shall fetch the stored session count on the laptimer")
{
    GIVEN("A setuped RestSessionManagementWorkflow")
    {
        auto restClient = RestClientMock{};
        auto rDl = RestSessionManagementWorkflow{&restClient};
        auto fetchedSignalSpy = SignalSpy{rDl.sessionCountFetched};
        auto restCall = std::make_shared<RestCallMock>();
        restCall->setData(SessionCountJson);

        REQUIRE_CALL(restClient, execute(_))
            .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path("/sessions"))
            .SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
            .LR_RETURN(restCall);

        WHEN("Fetching the SessionCount on the device")
        {
            rDl.fetchSessionCount();
            THEN("Then the correct session count shall be returned.")
            {
                constexpr auto sessionCount = std::size_t{2};
                REQUIRE(fetchedSignalSpy.getCount() == 1);
                REQUIRE(std::get<0>(fetchedSignalSpy.at(0)) == DownloadResult::Ok);
                REQUIRE(rDl.getSessionCount() == sessionCount);
            }
        }
    }
}

SCENARIO("The RestSessionManagementWorkflow shall download a specific session stored on the device")
{
    GIVEN("A setuped RestSessionDownloader")
    {
        auto restClient = RestClientMock{};
        auto rDl = RestSessionManagementWorkflow{&restClient};
        auto sessionDownloadSpy = SignalSpy{rDl.sessionDownloadFinshed};
        auto restCall = std::make_shared<RestCallMock>();

        REQUIRE_CALL(restClient, execute(_))
            .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path{"/sessions/0/data"})
            .SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
            .LR_RETURN(restCall);

        WHEN("Downloading the session with index 0")
        {
            restCall->setData(Sessions::getTestSessionAsJson());
            rDl.downloadSession(0);
            THEN("Then correct session shall be downloaded")
            {
                REQUIRE(sessionDownloadSpy.getCount() == 1);
                auto [index, downloadResult] = sessionDownloadSpy.at(0);
                REQUIRE(index == 0);
                REQUIRE(downloadResult == DownloadResult::Ok);
                REQUIRE(rDl.getSession(0).value_or(SessionData{}) == Sessions::getTestSession());
            }
        }
    }
}

TEST_CASE("The RestSessionManagementWorkflow shall download session metadata")
{
    auto restClient = RestClientMock{};
    auto rDl = RestSessionManagementWorkflow{&restClient};
    auto sessionMetadataDownloadSpy = SignalSpy{rDl.sessionMetadataDownloadFinished};
    auto restCall = std::make_shared<RestCallMock>();

    REQUIRE_CALL(restClient, execute(_))
        .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path{"/sessions/0/metadata"})
        .SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
        .LR_RETURN(restCall);

    restCall->setData(Sessions::getTestSessionMetaAsJson());
    rDl.downloadSessionMetadata(0);

    REQUIRE(sessionMetadataDownloadSpy.getCount() == 1);
    auto [index, downloadResult] = sessionMetadataDownloadSpy.at(0);
    REQUIRE(index == 0);
    REQUIRE(downloadResult == DownloadResult::Ok);
    REQUIRE(rDl.getSessionMetadata(0).value_or(SessionData{}) == Sessions::getTestSessionMetaData());
}

TEST_CASE("The RestSessionManagementWorkflow shall download all session meta data at once")
{
    auto restClient = RestClientMock{};
    auto rDl = RestSessionManagementWorkflow{&restClient};
    auto finishedSpy = SignalSpy{rDl.sessionMetadataDownloadFinished};
    auto sessionMetadataCall = std::make_shared<RestCallMock>();
    auto sessionCountCall = std::make_shared<RestCallMock>();

    REQUIRE_CALL(restClient, execute(_))
        .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path{"/sessions"})
        .SIDE_EFFECT(sessionCountCall->setCallResult(RestCallResult::Success))
        .LR_RETURN(sessionCountCall);

    REQUIRE_CALL(restClient, execute(_))
        .TIMES(2)
        .WITH(_1.getType() == RequestType::Get)
        .WITH(_1.getPath() == Path{"/sessions/0/metadata"} or _1.getPath() == Path{"/sessions/1/metadata"})
        .SIDE_EFFECT(sessionMetadataCall->setCallResult(RestCallResult::Success))
        .LR_RETURN(sessionMetadataCall);

    sessionCountCall->setData(SessionCountJson);
    sessionMetadataCall->setData(Sessions::getTestSessionMetaAsJson());
    rDl.downloadAllSessionMetadata();

    REQUIRE(finishedSpy.getCount() == 2);
    REQUIRE(rDl.getSessionMetadata(0).value_or(SessionMetaData{}) == Sessions::getTestSessionMetaData());
    REQUIRE(rDl.getSessionMetadata(1).value_or(SessionMetaData{}) == Sessions::getTestSessionMetaData());
}
