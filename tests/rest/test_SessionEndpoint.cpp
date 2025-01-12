// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rest/SessionEndpoint.hpp"
#include "testhelper/Sessions.hpp"
#include <testhelper/CompareHelper.hpp>
#include <testhelper/SessionDatabaseMock.hpp>
#include <testhelper/SignalSpy.hpp>

using namespace Rapid::Rest;
using namespace Rapid::TestHelper;
using namespace Rapid::Storage;
using namespace Rapid::System;

SCENARIO("Calling the Session endpoint /sessions with GET shall return the session count")
{
    GIVEN("A session endpoint with two session")
    {
        auto db = SessionDatabaseMock{};
        auto endpoint = SessionEndpoint{db};
        REQUIRE_CALL(db, getSessionCount()).RETURN(2);
        auto expectedReturnBody = std::string{R"({"count":2})"};
        auto finishedSpy = SignalSpy{endpoint.finished};

        WHEN("Requesting the top folder /sessions shall return the session count")
        {
            auto request = RestRequest{RequestType::Get, "/sessions"};
            endpoint.handleRestRequest(request);
            THEN("The finished signal is emitted and the correct return body is set")
            {
                REQUIRE(finishedSpy.getCount() == 1);
                auto [result, requestResult] = finishedSpy.at(0);
                REQUIRE(result == RequestHandleResult::Ok);
                REQUIRE(requestResult.getReturnBody() == expectedReturnBody);
            }
        }
    }
}

SCENARIO("Calling the Session endpoint with GET on a specific path under /sessions/{n}/data shall return the session")
{
    GIVEN("A session endpoint with one session")
    {
        auto db = SessionDatabaseMock{};
        auto endpoint = SessionEndpoint{db};
        auto finishedSpy = SignalSpy{endpoint.finished};

        WHEN("Successful request a session shall return the session")
        {
            auto asyncResult = std::make_shared<GetSessionResult>();
            asyncResult->setResultValue(Sessions::getTestSession());
            asyncResult->setResult(Result::Ok);
            REQUIRE_CALL(db, getSessionByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

            auto request = RestRequest{RequestType::Get, "/sessions/0/data"};
            endpoint.handleRestRequest(request);

            THEN("Give the correct session as json in the return body")
            {
                REQUIRE(finishedSpy.getCount() == 1);
                auto [result, resultRequest] = finishedSpy.at(0);
                REQUIRE(result == RequestHandleResult::Ok);
                REQUIRE(resultRequest.getReturnBody() == Sessions::getTestSessionAsJson());
            }
        }

        WHEN("Failed to request a session (db error) shall return an error")
        {
            auto asyncResult = std::make_shared<GetSessionResult>();
            asyncResult->setResult(Result::Error);
            REQUIRE_CALL(db, getSessionByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

            auto request = RestRequest{RequestType::Get, "/sessions/0/data"};
            endpoint.handleRestRequest(request);

            THEN("The request shall be set to a error")
            {
                REQUIRE(finishedSpy.getCount() == 1);
                auto [result, _] = finishedSpy.at(0);
                REQUIRE(result == RequestHandleResult::Error);
            }
        }
    }
}

TEST_CASE(
    "Calling the Session endpoint with GET on a specific path under /sessions/{n}/metadata shall return the session")
{
    auto db = SessionDatabaseMock{};
    auto endpoint = SessionEndpoint{db};
    auto finishedSpy = SignalSpy{endpoint.finished};

    SECTION("Valied request")
    {
        auto asyncResult = std::make_shared<GetSessionMetaDataResult>();
        asyncResult->setResultValue(Sessions::getTestSessionMetaData());
        asyncResult->setResult(Result::Ok);
        REQUIRE_CALL(db, getSessionMetaDataByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

        auto request = RestRequest{RequestType::Get, "/sessions/0/metadata"};
        endpoint.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, resultRequest] = finishedSpy.at(0);
        REQUIRE(result == RequestHandleResult::Ok);
        REQUIRE(resultRequest.getReturnBody() == Sessions::getTestSessionMetaAsJson());
    }

    SECTION("Invalid request")
    {
        auto asyncResult = std::make_shared<GetSessionMetaDataResult>();
        asyncResult->setResult(Result::Error);
        REQUIRE_CALL(db, getSessionMetaDataByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

        auto request = RestRequest{RequestType::Get, "/sessions/0/metadata"};
        endpoint.handleRestRequest(request);

        REQUIRE(finishedSpy.getCount() == 1);
        auto [result, _] = finishedSpy.at(0);
        REQUIRE(result == RequestHandleResult::Error);
    }
}

TEST_CASE("Calling the Session endpoint with DELETE on a specific path under /sessions/{n} shall delete the session")
{
    auto db = SessionDatabaseMock{};
    auto endpoint = SessionEndpoint{db};
    auto finishedSpy = SignalSpy{endpoint.finished};

    REQUIRE_CALL(db, deleteSession(trompeloeil::_)).WITH(_1 == 0);

    auto request = RestRequest{RequestType::Delete, "/sessions/0"};
    endpoint.handleRestRequest(request);

    REQUIRE(finishedSpy.getCount() == 1);
    auto [result, _] = finishedSpy.at(0);
    REQUIRE(result == RequestHandleResult::Ok);
}
