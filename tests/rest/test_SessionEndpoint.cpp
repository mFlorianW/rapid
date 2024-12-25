// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionEndpoint.hpp"
#include "Sessions.hpp"
#include <CompareHelper.hpp>
#include <SessionDatabaseMock.hpp>

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
        bool finished = false;
        auto expectedReturnBody = std::string{R"({"count":2})"};
        std::string returnBody;
        std::ignore = endpoint.finished.connect([&finished, &returnBody](auto&& result, auto&& request) {
            REQUIRE(result == RequestHandleResult::Ok);
            returnBody = request.getReturnBody();
            finished = true;
        });

        WHEN("Requesting the top folder /sessions shall return the session count")
        {
            auto request = RestRequest{RequestType::Get, "/sessions"};
            endpoint.handleRestRequest(request);
            THEN("The finished signal is emitted and the correct return body is set")
            {
                // REQUIRE_COMPARE_WITH_TIMEOUT(finished, true, std::chrono::milliseconds{1});
                REQUIRE(finished == true);
                REQUIRE(returnBody == expectedReturnBody);
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

        WHEN("Successful request a session shall return the session")
        {
            auto asyncResult = std::make_shared<GetSessionResult>();
            asyncResult->setResultValue(Sessions::getTestSession());
            asyncResult->setResult(Result::Ok);
            bool finished = false;

            std::ignore = endpoint.finished.connect([&finished](auto result, auto&& request) {
                finished = true;
                auto expectedReturnBody = Sessions::getTestSessionAsJson();
                REQUIRE(request.getReturnBody() == expectedReturnBody);
            });

            REQUIRE_CALL(db, getSessionByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

            auto request = RestRequest{RequestType::Get, "/sessions/0/data"};
            endpoint.handleRestRequest(request);
            THEN("Give the correct session as json in the return body")
            {
                REQUIRE_COMPARE_WITH_TIMEOUT(finished, true, std::chrono::milliseconds{1});
            }
        }

        WHEN("Failed to request a session (db error) shall return an error")
        {
            auto asyncResult = std::make_shared<GetSessionResult>();
            asyncResult->setResult(Result::Error);
            bool finished = false;

            std::ignore = endpoint.finished.connect([&finished](auto result, auto&& request) {
                finished = true;
                REQUIRE(result == RequestHandleResult::Error);
            });

            REQUIRE_CALL(db, getSessionByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

            auto request = RestRequest{RequestType::Get, "/sessions/0/data"};
            endpoint.handleRestRequest(request);
            THEN("The request shall be set to a error")
            {
                REQUIRE(finished);
            }
        }
    }
}

TEST_CASE(
    "Calling the Session endpoint with GET on a specific path under /sessions/{n}/metadata shall return the session")
{
    auto db = SessionDatabaseMock{};
    auto endpoint = SessionEndpoint{db};

    SECTION("Valied request")
    {
        auto asyncResult = std::make_shared<GetSessionMetaDataResult>();
        asyncResult->setResultValue(Sessions::getTestSessionMetaData());
        asyncResult->setResult(Result::Ok);
        bool finished = false;
        auto returnedBody = std::string{};

        std::ignore = endpoint.finished.connect([&finished, &returnedBody](auto result, auto&& request) {
            finished = true;
            REQUIRE(result == RequestHandleResult::Ok);
            returnedBody = request.getReturnBody();
        });

        REQUIRE_CALL(db, getSessionMetaDataByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

        auto request = RestRequest{RequestType::Get, "/sessions/0/metadata"};
        endpoint.handleRestRequest(request);

        REQUIRE(finished);
        REQUIRE(returnedBody == Sessions::getTestSessionMetaAsJson());
    }

    SECTION("Invalid request")
    {
        auto asyncResult = std::make_shared<GetSessionMetaDataResult>();
        asyncResult->setResult(Result::Error);
        bool finished = false;

        std::ignore = endpoint.finished.connect([&finished](auto result, auto&& request) {
            finished = true;
            REQUIRE(result == RequestHandleResult::Error);
        });

        REQUIRE_CALL(db, getSessionMetaDataByIndexAsync(trompeloeil::_)).WITH(_1 == 0).LR_RETURN(asyncResult);

        auto request = RestRequest{RequestType::Get, "/sessions/0/metadata"};
        endpoint.handleRestRequest(request);
        THEN("The request shall be set to a error")
        {
            REQUIRE(finished);
        }
    }
}

TEST_CASE("Calling the Session endpoint with DELETE on a specific path under /sessions/{n} shall delete the session")
{
    auto db = SessionDatabaseMock{};
    auto endpoint = SessionEndpoint{db};

    REQUIRE_CALL(db, deleteSession(trompeloeil::_)).WITH(_1 == 0);

    auto request = RestRequest{RequestType::Delete, "/sessions/0"};
    endpoint.handleRestRequest(request);
}
