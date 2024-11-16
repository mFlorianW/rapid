// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Sessions.hpp"
#include "SqliteSessionDatabase.hpp"
#include "private/Connection.hpp"
#include <SqliteDatabaseTestHelper.hpp>
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <sqlite3.h>

using namespace Rapid::System;
using namespace Rapid::Storage;
using namespace Rapid::TestHelper;
using namespace Rapid::TestHelper::SqliteDatabaseTestHelper;
using namespace Rapid::Storage::Private;
using namespace Rapid::Common;

namespace
{

class TestSqliteSessionDatabaseEventListener : public Catch::EventListenerBase
{
    using Catch::EventListenerBase::EventListenerBase;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        // For the case the test crashes.
        if (std::filesystem::exists(getTestDatabseFile("test_session.db")) == true) {
            dropSessionData();
        } else {
            if (!std::filesystem::exists(getTestDatabseFolder())) {
                REQUIRE(std::filesystem::create_directory(getTestDatabseFolder()) == true);
            }
            auto const cleanDbFile = getWorkingDir() + "/test_session.db";
            REQUIRE(std::filesystem::copy_file(cleanDbFile, getTestDatabseFile("test_session.db")) == true);
        }
    }

    void testCaseEnded(Catch::TestCaseStats const& testCaseStatu) override
    {
        dropSessionData();
    }

    void dropSessionData()
    {
        auto* dbCon = Connection::connection(getTestDatabseFile("test_session.db")).getRawHandle();
        auto const deleted = sqlite3_exec(dbCon, "DELETE FROM Session", nullptr, nullptr, nullptr) == SQLITE_OK;
        if (not deleted) {
            std::cerr << "Failed to delete session information. Error: " << sqlite3_errmsg(dbCon) << "\n";
            FAIL("The session database parts are not reseted.");
        }
    }
};
} // namespace

CATCH_REGISTER_LISTENER(TestSqliteSessionDatabaseEventListener)

TEST_CASE("The SqliteSessionDatabase shall store as session and shall emit the session added signal.")
{
    auto db = SqliteSessionDatabase{getTestDatabseFile("test_session.db")};
    auto sessionAddedSignalEmitted = false;
    auto addedIndex = std::size_t{123456};
    db.sessionAdded.connect([&sessionAddedSignalEmitted, &addedIndex](std::size_t index) {
        sessionAddedSignalEmitted = true;
        addedIndex = index;
    });

    auto insertResult = db.storeSession(Sessions::getTestSession3());
    insertResult->waitForFinished();
    REQUIRE(insertResult->getResult() == Result::Ok);
    REQUIRE(addedIndex == 0);
    REQUIRE(sessionAddedSignalEmitted == true);

    sessionAddedSignalEmitted = false;
    insertResult = db.storeSession(Sessions::getTestSession4());
    insertResult->waitForFinished();
    REQUIRE(insertResult->getResult() == Result::Ok);
    REQUIRE(addedIndex == 1);
    REQUIRE(sessionAddedSignalEmitted == true);
}

TEST_CASE("The SqliteSessionDatabase shall store a session and the session shall be directly read again.")
{
    auto db = SqliteSessionDatabase{getTestDatabseFile("test_session.db")};
    auto addedIndex = std::size_t{123456};
    db.sessionAdded.connect([&addedIndex](std::size_t index) {
        addedIndex = index;
    });

    auto const insertResult = db.storeSession(Sessions::getTestSession3());
    insertResult->waitForFinished();
    REQUIRE(insertResult->getResult() == Result::Ok);
    REQUIRE(addedIndex == 0);

    auto const readResult = db.getSessionByIndex(addedIndex);
    REQUIRE(readResult.has_value() == true);
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(readResult.value() == Sessions::getTestSession3());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("The SqliteSessionDatabase shall store a already stored session under the same index and shall emit session "
          "updated.")
{
    auto db = SqliteSessionDatabase{getTestDatabseFile("test_session.db")};
    auto updatedIndex = std::size_t{123456};
    db.sessionUpdated.connect([&updatedIndex](std::size_t index) {
        updatedIndex = index;
    });

    auto insertResult = db.storeSession(Sessions::getTestSession3());
    insertResult->waitForFinished();
    REQUIRE(insertResult->getResult() == Result::Ok);
    insertResult = db.storeSession(Sessions::getTestSession4());
    insertResult->waitForFinished();
    REQUIRE(insertResult->getResult() == Result::Ok);

    auto lapData = Rapid::Common::LapData{};
    lapData.addSectorTimes({{"00:23:32.003"}, {"00:23:32.004"}, {"00:23:32.005"}});
    auto session1 = Sessions::getTestSession3();
    session1.addLap(lapData);
    auto session2 = Sessions::getTestSession4();
    session2.addLap(lapData);

    insertResult = db.storeSession(session1);
    insertResult->waitForFinished();
    REQUIRE(insertResult->getResult() == Result::Ok);
    REQUIRE(updatedIndex == 0);
    REQUIRE(db.getSessionByIndex(0) == session1);

    insertResult = db.storeSession(session2);
    insertResult->waitForFinished();
    REQUIRE(insertResult->getResult() == Result::Ok);
    REQUIRE(updatedIndex == 1);
    REQUIRE(db.getSessionByIndex(1) == session2);
}

TEST_CASE("The SqliteSessionDatabase shall gives the number of stored sessions and should return the correct session"
          "for that index.")
{
    auto db = SqliteSessionDatabase{getTestDatabseFile("test_session.db")};
    auto const session1 = Sessions::getTestSession3();
    auto const session2 = Sessions::getTestSession4();
    constexpr auto expectedSessionCount = 2;

    // Prepare database.
    auto storeResult = db.storeSession(session1);
    storeResult->waitForFinished();
    REQUIRE(storeResult->getResult() == Result::Ok);
    storeResult = db.storeSession(session2);
    storeResult->waitForFinished();
    REQUIRE(storeResult->getResult() == Result::Ok);

    auto const sessionCount = db.getSessionCount();
    REQUIRE(sessionCount == expectedSessionCount);

    REQUIRE(db.getSessionByIndex(0).value_or(SessionData{}) == session1);
    REQUIRE(db.getSessionByIndex(1).value_or(SessionData{}) == session2);
    REQUIRE(db.getSessionByIndex(2) == std::nullopt);
}

TEST_CASE("The SqliteSessionDatabase shall delete a session under the index and shall emit the sessionDelete signal "
          "with the index.")
{
    auto db = SqliteSessionDatabase{getTestDatabseFile("test_session.db")};
    auto const session1 = Sessions::getTestSession3();
    auto const session2 = Sessions::getTestSession4();
    auto deletedIndex = std::size_t{123456};
    constexpr auto indexToDelete = 1;
    db.sessionDeleted.connect([&deletedIndex](std::size_t index) {
        deletedIndex = index;
    });

    // Prepare database.
    auto storeResult = db.storeSession(session1);
    storeResult->waitForFinished();
    REQUIRE(storeResult->getResult() == Result::Ok);
    storeResult = db.storeSession(session2);
    storeResult->waitForFinished();
    REQUIRE(storeResult->getResult() == Result::Ok);
    REQUIRE(db.getSessionCount() == 2);

    db.deleteSession(indexToDelete);
    REQUIRE(deletedIndex == indexToDelete);
    REQUIRE(db.getSessionCount() == 1);
}

TEST_CASE("The SqlieSessionDatabase shall emit session deteled on referential integrity changes")
{
    auto db = SqliteSessionDatabase{getTestDatabseFile("test_session.db")};
    auto const session1 = Sessions::getTestSession3();
    auto deletedIndex = std::size_t{123456};
    constexpr auto indexToDelete = 0;

    auto storeResult = db.storeSession(session1);
    storeResult->waitForFinished();
    REQUIRE(storeResult->getResult() == Result::Ok);
    db.sessionDeleted.connect([&deletedIndex](std::size_t index) {
        deletedIndex = index;
    });

    // Delete the Oschersleben Track these commands should trigger the referential integrity changes
    // and that should also delete session in the database.
    auto* dbCon = Connection::connection(getTestDatabseFile("test_session.db")).getRawHandle();
    REQUIRE(sqlite3_exec(dbCon, "DELETE FROM Track WHERE Track.Name = 'Oschersleben'", nullptr, nullptr, nullptr) ==
            SQLITE_OK);

    REQUIRE(deletedIndex == indexToDelete);

    // Restore the deleted track
    // clang-format off
    constexpr auto osl = \
        "-- INSERT OSCHERSLEBEN TRACK\n"
        "INSERT INTO Position (Latitude, Longitude) VALUES\n"
        "   (52.0258333, 11.279166666), -- Finishline Position\n"
        "  (52.0258333, 11.279166666), -- Startline Position\n"
        "   (52.0258333, 11.279166666), -- Sektor1 Position\n"
        "   (52.258335, 11.279166666); -- Sektor2 Position\n"
        "-- INSERT INTO TRACKS (Longitude, Latitude)\n"
        "INSERT INTO Track (Name, Finishline, Startline) VALUES\n"
        "    ('Oschersleben', (SELECT PositionId FROM Position WHERE Latitude = 52.0258333 AND Longitude = 11.279166666), (SELECT PositionId FROM Position WHERE Latitude = 52.0258333 AND Longitude = 11.279166666));\n"
        "-- SECTORS\n"
        "-- SEKTOR1\n"
        "INSERT INTO Sektor (TrackId, PositionId, SektorIndex)\n"
        "    VALUES\n"
        "    ((SELECT TrackId FROM Track WHERE name = 'Oschersleben'), (SELECT PositionId FROM Position WHERE Latitude = 52.0258333 AND Longitude = 11.279166666), 1);\n"
        "-- SEKTOR2\n"
        "INSERT INTO Sektor (TrackId, PositionId, SektorIndex)\n"
        "    VALUES\n"
        "    ((SELECT TrackId FROM Track WHERE NAME = 'Oschersleben'), (SELECT PositionId FROM Position WHERE Latitude = 52.258335 AND Longitude = 11.279166666), 2);\n";
    // clang-format on
    REQUIRE(sqlite3_exec(dbCon, osl, nullptr, nullptr, nullptr) == SQLITE_OK);
}
