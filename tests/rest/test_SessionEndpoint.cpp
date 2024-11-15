// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MemorySessionDatabaseBackend.hpp"
#include "SessionDatabase.hpp"
#include "SessionEndpoint.hpp"
#include "Sessions.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Rest;
using namespace Rapid::TestHelper;
using namespace Rapid::Storage;

SCENARIO("Calling the Session endpoint /sessions with GET shall return the session count")
{
    GIVEN("A session endpoint with two session")
    {
        auto dbBackend = MemorySessionDatabaseBackend{};
        auto db = SessionDatabase{dbBackend};
        auto endpoint = SessionEndpoint{db};
        auto result = db.storeSession(Sessions::getTestSession());
        result->waitForFinished();
        result = db.storeSession(Sessions::getTestSession2());
        result->waitForFinished();

        WHEN("Requesting the top folder /sessions shall return the session count")
        {
            auto request = RestRequest{RequestType::Get, "/sessions"};
            endpoint.handleRestRequest(request);
            THEN("Give the correct session id list.")
            {
                auto expectedReturnBody = std::string{R"({"count":2})"};
                REQUIRE(request.getReturnBody() == expectedReturnBody);
            }
        }
    }
}

SCENARIO("Calling the Session endpoint with GET on a specific path under /sessions/{n} shall return the session")
{
    GIVEN("A session endpoint with one session")
    {
        auto dbBackend = MemorySessionDatabaseBackend{};
        auto db = SessionDatabase{dbBackend};
        auto endpoint = SessionEndpoint{db};
        auto asyncResult = db.storeSession(Sessions::getTestSession());
        asyncResult->waitForFinished();

        WHEN("Requesting the a specifc under /sessions/0 shall return the session")
        {
            auto request = RestRequest{RequestType::Get, "/sessions/0"};
            endpoint.handleRestRequest(request);
            THEN("Give the correct session as json in the return body")
            {
                auto expectedReturnBody = Sessions::getTestSessionAsJson();
                REQUIRE(request.getReturnBody() == expectedReturnBody);
            }
        }
    }
}

SCENARIO("Calling the Session endpoint with DELETE on a specific path under /sessions/{n} shall delete the session")
{
    GIVEN("A session endpoint with one session")
    {
        auto dbBackend = MemorySessionDatabaseBackend{};
        auto db = SessionDatabase{dbBackend};
        auto endpoint = SessionEndpoint{db};
        auto asyncResult = db.storeSession(Sessions::getTestSession());
        asyncResult->waitForFinished();

        WHEN("The endpoint with a DELETE request is called")
        {
            auto request = RestRequest{RequestType::Delete, "/sessions/0"};
            endpoint.handleRestRequest(request);
            THEN("The session under the index shall be deleted")
            {
                REQUIRE(db.getSessionCount() == 0);
            }
        }
    }
}
