// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MemorySessionDatabaseBackend.hpp"
#include "SessionDatabase.hpp"
#include "Sessions.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::System;
using namespace Rapid::Storage;
using namespace Rapid::Common;
using namespace Rapid::TestHelper;

TEST_CASE("The SessionDatabase shall serialize the SessionData to JSON, store them and emit the signal new session is "
          "stored.")
{
    auto backend = MemorySessionDatabaseBackend{};
    auto sessionDb = SessionDatabase{backend};
    auto sessionStoredSpy = false;
    auto sessionStoredIndex = std::size_t{99};
    sessionDb.sessionAdded.connect([&](std::size_t index) {
        sessionStoredSpy = true;
        sessionStoredIndex = index;
    });
    auto result = sessionDb.storeSession(Sessions::getTestSession());

    REQUIRE(result->getResult() == Result::Ok);
    REQUIRE(sessionStoredSpy);
    REQUIRE(sessionStoredIndex == 0);
    REQUIRE(backend.loadSessionByIndex(0) == Sessions::getTestSessionAsJson());
}

TEST_CASE("The SessionDatabase shall return the amount of the stored sessions")
{
    auto backend = MemorySessionDatabaseBackend{};
    auto sessionDb = SessionDatabase{backend};

    auto result = sessionDb.storeSession(Sessions::getTestSession());

    REQUIRE(result->getResult() == Result::Ok);
    REQUIRE(sessionDb.getSessionCount() == 1);
}

TEST_CASE("The SessionDatabase shall store two different sessions")
{
    auto backend = MemorySessionDatabaseBackend{};
    auto sessionDb = SessionDatabase{backend};

    auto result1 = sessionDb.storeSession(Sessions::getTestSession());
    auto result2 = sessionDb.storeSession(Sessions::getTestSession2());

    REQUIRE(result1->getResult() == Result::Ok);
    REQUIRE(result2->getResult() == Result::Ok);
    REQUIRE(backend.getNumberOfStoredSessions() == 2);
    REQUIRE(sessionDb.getSessionCount() == 2);
    REQUIRE(sessionDb.getSessionByIndex(0) == Sessions::getTestSession());
    REQUIRE(sessionDb.getSessionByIndex(1) == Sessions::getTestSession2());
}

TEST_CASE("The SessionDatabase shall be able to delete a single session and emit the signal session deleted.")
{
    auto backend = MemorySessionDatabaseBackend{};
    auto sessionDb = SessionDatabase{backend};
    auto sessionDeletedSpy = false;
    auto sessionDeletedIndex = std::size_t{99};

    auto result = sessionDb.storeSession(Sessions::getTestSession());
    sessionDb.sessionDeleted.connect([&](std::size_t index) {
        sessionDeletedSpy = true;
        sessionDeletedIndex = index;
    });
    REQUIRE(sessionDb.getSessionCount() == 1);
    REQUIRE(result->getResult() == Result::Ok);

    sessionDb.deleteSession(0);
    REQUIRE(sessionDb.getSessionCount() == 0);
    REQUIRE(sessionDeletedSpy);
    REQUIRE(sessionDeletedIndex == 0);
}

TEST_CASE("The SessionDatabase shall load the Session by the given valid index.")
{
    auto backend = MemorySessionDatabaseBackend{};
    auto sessionDb = SessionDatabase{backend};

    auto result = sessionDb.storeSession(Sessions::getTestSession());
    auto session = sessionDb.getSessionByIndex(0);

    REQUIRE(result->getResult() == Result::Ok);
    REQUIRE(session.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(session.value() == Sessions::getTestSession());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("The SessionDatabase shall store an already stored session under the same index. and shall emit the signal "
          "session updated.")
{
    auto backend = MemorySessionDatabaseBackend{};
    auto sessionDb = SessionDatabase{backend};
    auto sessionUpdatedSpy = false;
    auto sessionUpdatedIndex = std::size_t{99};
    sessionDb.sessionUpdated.connect([&](std::size_t index) {
        sessionUpdatedSpy = true;
        sessionUpdatedIndex = index;
    });

    auto result1 = sessionDb.storeSession(Sessions::getTestSession());
    auto result2 = sessionDb.storeSession(Sessions::getTestSession2());
    auto result3 = sessionDb.storeSession(Sessions::getTestSession2());

    REQUIRE(result1->getResult() == Result::Ok);
    REQUIRE(result2->getResult() == Result::Ok);
    REQUIRE(result3->getResult() == Result::Ok);
    REQUIRE(sessionDb.getSessionCount() == 2);
    REQUIRE(sessionDb.getSessionByIndex(0) == Sessions::getTestSession());
    REQUIRE(sessionDb.getSessionByIndex(1) == Sessions::getTestSession2());
    REQUIRE(sessionUpdatedSpy);
    REQUIRE(sessionUpdatedIndex == 1);
}
