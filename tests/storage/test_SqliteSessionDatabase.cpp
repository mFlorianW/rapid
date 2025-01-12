// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "storage/SqliteSessionDatabase.hpp"
#include "storage/private/Connection.hpp"
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <testhelper/CompareHelper.hpp>
#include <testhelper/Sessions.hpp>
#include <testhelper/SqliteDatabaseTestHelper.hpp>

using namespace Rapid::System;
using namespace Rapid::Storage;
using namespace Rapid::TestHelper;
using namespace Rapid::TestHelper::SqliteDatabaseTestHelper;
using namespace Rapid::Storage::Private;
using namespace Rapid::Common;
using namespace std::chrono_literals;

namespace
{

class SqliteSessionDatabaseEventListener : public SqliteDatabaseTestEventlistener
{
    using SqliteDatabaseTestEventlistener::SqliteDatabaseTestEventlistener;

    [[nodiscard]] std::string getCleanDbFileName() const noexcept override
    {
        static auto dbFile = std::string{"test_session.db"};
        return dbFile;
    }
};

struct TestFixture
{
    SessionData session1 = Sessions::getTestSession3();
    SessionData session2 = Sessions::getTestSession4();

    void setupTestDatabase(SqliteSessionDatabase& db)
    {
        storeSession1(db);
        storeSession2(db);
    }

    void storeSession1(SqliteSessionDatabase& db)
    {
        auto const session1 = Sessions::getTestSession3();
        auto storeResult = db.storeSession(session1);
        storeResult->waitForFinished();
        REQUIRE(storeResult->getResult() == Result::Ok);
    }

    void storeSession2(SqliteSessionDatabase& db)
    {
        auto const session2 = Sessions::getTestSession4();
        auto storeResult = db.storeSession(session2);
        storeResult->waitForFinished();
        REQUIRE(storeResult->getResult() == Result::Ok);
    }
};

} // namespace

CATCH_REGISTER_LISTENER(SqliteSessionDatabaseEventListener)

TEST_CASE_METHOD(TestFixture,
                 "The SqliteSessionDatabase shall store as session and shall emit the session added signal.")
{
    auto sessionAddedSignalEmitted = false;
    auto addedIndex = std::size_t{123456};

    SECTION("Store session and emit signal")
    {
        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        std::ignore = db.sessionAdded.connect([&sessionAddedSignalEmitted, &addedIndex](std::size_t index) {
            sessionAddedSignalEmitted = true;
            addedIndex = index;
        });
        storeSession1(db);
        REQUIRE(addedIndex == 0);
        REQUIRE(sessionAddedSignalEmitted == true);

        sessionAddedSignalEmitted = false;
        storeSession2(db);
        REQUIRE(addedIndex == 1);
        REQUIRE(sessionAddedSignalEmitted == true);
    }

    SECTION("Store session and all instances shall emit the signal")
    {
        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        auto db2 = SqliteSessionDatabase{getTestDatabaseFile()};
        std::ignore = db2.sessionAdded.connect([&sessionAddedSignalEmitted, &addedIndex](std::size_t index) {
            sessionAddedSignalEmitted = true;
            addedIndex = index;
        });

        storeSession1(db);
        REQUIRE(addedIndex == 0);
        REQUIRE(sessionAddedSignalEmitted == true);
    }
}

TEST_CASE_METHOD(TestFixture,
                 "The SqliteSessionDatabase shall store a session and the session shall be directly read again.")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    auto addedIndex = std::size_t{123456};
    std::ignore = db.sessionAdded.connect([&addedIndex](std::size_t index) {
        addedIndex = index;
    });

    storeSession1(db);
    REQUIRE(addedIndex == 0);

    auto const readResult = db.getSessionByIndex(addedIndex);
    REQUIRE(readResult.has_value() == true);
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(readResult.value() == session1);

    auto const asyncReadResult = db.getSessionByIndexAsync(addedIndex);
    REQUIRE_COMPARE_WITH_TIMEOUT(asyncReadResult->getResult(), Result::Ok, std::chrono::milliseconds{10});
    REQUIRE(asyncReadResult->getResult() == Result::Ok);
    REQUIRE(asyncReadResult->getResultValue().has_value());
    REQUIRE(asyncReadResult->getResultValue().value() == Sessions::getTestSession3());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE_METHOD(
    TestFixture,
    "The SqliteSessionDatabase shall store a already stored session under the same index and shall emit session "
    "updated.")
{
    auto lapData = Rapid::Common::LapData{};
    lapData.addSectorTimes({{"00:23:32.003"}, {"00:23:32.004"}, {"00:23:32.005"}});
    SECTION("Update session and emit the signal")
    {
        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        auto updatedIndex = std::size_t{123456};
        std::ignore = db.sessionUpdated.connect([&updatedIndex](std::size_t index) {
            updatedIndex = index;
        });

        setupTestDatabase(db);
        auto session1 = Sessions::getTestSession3();
        session1.addLap(lapData);
        auto session2 = Sessions::getTestSession4();
        session2.addLap(lapData);

        auto insertResult = db.storeSession(session1);
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

    SECTION("The updated signal is emitted from every database instance")
    {
        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        auto db2 = SqliteSessionDatabase{getTestDatabaseFile()};
        auto updatedIndex = std::size_t{123456};
        std::ignore = db2.sessionUpdated.connect([&updatedIndex](std::size_t index) {
            updatedIndex = index;
        });

        session1.addLap(lapData);

        auto insertResult = db.storeSession(session1);
        REQUIRE_COMPARE_WITH_TIMEOUT(insertResult->getResult(), Result::Ok, 1s);
        CHECK(insertResult->getResult() == Result::Ok);
        CHECK(updatedIndex == 0);
        REQUIRE(db.getSessionByIndex(0) == session1);
    }
}

TEST_CASE_METHOD(
    TestFixture,
    "The SqliteSessionDatabase shall gives the number of stored sessions and should return the correct session"
    "for that index.")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    setupTestDatabase(db);
    SECTION("Correct session count is returned")
    {
        constexpr auto expectedSessionCount = 2;
        auto const sessionCount = db.getSessionCount();
        REQUIRE(sessionCount == expectedSessionCount);
    }

    SECTION("Correct session data is returned")
    {
        REQUIRE(db.getSessionByIndex(0).value_or(SessionData{}) == session1);
        REQUIRE(db.getSessionByIndex(1).value_or(SessionData{}) == session2);
        REQUIRE(db.getSessionByIndex(2) == std::nullopt);
    }
}

TEST_CASE_METHOD(
    TestFixture,
    "The SqliteSessionDatabase shall delete a session under the index and shall emit the sessionDelete signal "
    "with the index.")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    setupTestDatabase(db);
    auto deletedIndex = std::size_t{123456};
    constexpr auto indexToDelete = 1;
    std::ignore = db.sessionDeleted.connect([&deletedIndex](std::size_t index) {
        deletedIndex = index;
    });

    db.deleteSession(indexToDelete);
    REQUIRE(deletedIndex == indexToDelete);
    REQUIRE(db.getSessionCount() == 1);
}

TEST_CASE_METHOD(TestFixture, "The SqlieSessionDatabase shall emit session deteled on referential integrity changes")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    storeSession1(db);
    auto deletedIndex = std::size_t{123456};
    constexpr auto indexToDelete = 0;
    std::ignore = db.sessionDeleted.connect([&deletedIndex](std::size_t index) {
        deletedIndex = index;
    });

    // Delete the Oschersleben Track these commands should trigger the referential integrity changes
    // and that should also delete session in the database.
    auto* dbCon = Connection::connection(getTestDatabaseFile())->getRawHandle();
    CHECK(sqlite3_exec(dbCon, "DELETE FROM Track WHERE Track.Name = 'Oschersleben'", nullptr, nullptr, nullptr) ==
          SQLITE_OK);
    REQUIRE(deletedIndex == indexToDelete);
}

TEST_CASE_METHOD(TestFixture, "The SqliteSessionDatabase shall give the session meta data for a index.")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    setupTestDatabase(db);

    auto loadResult = db.getSessionMetaDataByIndexAsync(0);
    REQUIRE_COMPARE_WITH_TIMEOUT(loadResult->getResult(), Rapid::System::Result::Ok, std::chrono::seconds{1});
    CHECK(loadResult->getResultValue().value_or(SessionMetaData{}).getSessionDate() == session1.getSessionDate());
    CHECK(loadResult->getResultValue().value_or(SessionMetaData{}).getSessionTime() == session1.getSessionTime());
    CHECK(loadResult->getResultValue().value_or(SessionMetaData{}).getTrack() == session1.getTrack());

    loadResult = db.getSessionMetaDataByIndexAsync(1);
    REQUIRE_COMPARE_WITH_TIMEOUT(loadResult->getResult(), Rapid::System::Result::Ok, std::chrono::seconds{1});
    CHECK(loadResult->getResultValue().value_or(SessionMetaData{}).getSessionDate() == session2.getSessionDate());
    CHECK(loadResult->getResultValue().value_or(SessionMetaData{}).getSessionTime() == session2.getSessionTime());
    CHECK(loadResult->getResultValue().value_or(SessionMetaData{}).getTrack() == session2.getTrack());
}

TEST_CASE_METHOD(TestFixture, "The SqliteSessionDatabase shall give the session data for session meta data")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    setupTestDatabase(db);

    auto loadResult = db.getSessionByMetadataAsync(session1);
    REQUIRE_COMPARE_WITH_TIMEOUT(loadResult->getResult(), Rapid::System::Result::Ok, std::chrono::seconds{1});
    CHECK(loadResult->getResultValue().value_or(SessionData{}) == session1);

    loadResult = db.getSessionByMetadataAsync(session2);
    REQUIRE_COMPARE_WITH_TIMEOUT(loadResult->getResult(), Rapid::System::Result::Ok, std::chrono::seconds{1});
    CHECK(loadResult->getResultValue().value_or(SessionData{}) == session2);
}
