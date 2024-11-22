// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionEndpoint.hpp"
#include "Sessions.hpp"
#include <SessionDatabaseMock.hpp>

using namespace Rapid::Rest;
using namespace Rapid::TestHelper;
using namespace Rapid::Storage;

SCENARIO("Calling the Session endpoint /sessions with GET shall return the session count")
{
    GIVEN("A session endpoint with two session")
    {
        auto db = SessionDatabaseMock{};
        auto endpoint = SessionEndpoint{db};
        REQUIRE_CALL(db, getSessionCount()).RETURN(2);

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
        auto db = SessionDatabaseMock{};
        auto endpoint = SessionEndpoint{db};

        REQUIRE_CALL(db, getSessionByIndex(trompeloeil::_)).WITH(_1 == 0).RETURN(Sessions::getTestSession());

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

TEST_CASE("Calling the Session endpoint with DELETE on a specific path under /sessions/{n} shall delete the session")
{
    auto db = SessionDatabaseMock{};
    auto endpoint = SessionEndpoint{db};

    REQUIRE_CALL(db, deleteSession(trompeloeil::_)).WITH(_1 == 0);

    auto request = RestRequest{RequestType::Delete, "/sessions/0"};
    endpoint.handleRestRequest(request);
}
