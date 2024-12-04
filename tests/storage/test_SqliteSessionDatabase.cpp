// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Sessions.hpp"
#include "SqliteSessionDatabase.hpp"
#include "private/Connection.hpp"
#include <CompareHelper.hpp>
#include <SqliteDatabaseTestHelper.hpp>
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>

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

} // namespace

CATCH_REGISTER_LISTENER(SqliteSessionDatabaseEventListener)

TEST_CASE("The SqliteSessionDatabase shall store as session and shall emit the session added signal.")
{

    SECTION("Store session and emit signal")
    {

        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        auto sessionAddedSignalEmitted = false;
        auto addedIndex = std::size_t{123456};
        std::ignore = db.sessionAdded.connect([&sessionAddedSignalEmitted, &addedIndex](std::size_t index) {
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

    SECTION("Store session and all instances shall emit the signal")
    {

        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        auto db2 = SqliteSessionDatabase{getTestDatabaseFile()};
        auto sessionAddedSignalEmitted = false;
        auto addedIndex = std::size_t{123456};
        std::ignore = db2.sessionAdded.connect([&sessionAddedSignalEmitted, &addedIndex](std::size_t index) {
            sessionAddedSignalEmitted = true;
            addedIndex = index;
        });

        auto insertResult = db.storeSession(Sessions::getTestSession3());
        REQUIRE_COMPARE_WITH_TIMEOUT(insertResult->getResult(), Result::Ok, 1s);
        insertResult->waitForFinished();
        REQUIRE(insertResult->getResult() == Result::Ok);
        REQUIRE(addedIndex == 0);
        REQUIRE(sessionAddedSignalEmitted == true);
    }
}

TEST_CASE("The SqliteSessionDatabase shall store a session and the session shall be directly read again.")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    auto addedIndex = std::size_t{123456};
    std::ignore = db.sessionAdded.connect([&addedIndex](std::size_t index) {
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

    auto const asyncReadResult = db.getSessionByIndexAsync(addedIndex);
    REQUIRE_COMPARE_WITH_TIMEOUT(asyncReadResult->getResult(), Result::Ok, std::chrono::milliseconds{10});
    REQUIRE(asyncReadResult->getResult() == Result::Ok);
    REQUIRE(asyncReadResult->getResultValue().has_value());
    REQUIRE(asyncReadResult->getResultValue().value() == Sessions::getTestSession3());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("The SqliteSessionDatabase shall store a already stored session under the same index and shall emit session "
          "updated.")
{
    SECTION("Update session and emit the signal")
    {
        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        auto updatedIndex = std::size_t{123456};
        std::ignore = db.sessionUpdated.connect([&updatedIndex](std::size_t index) {
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

    SECTION("The updated signal is emitted from every database instance")
    {
        auto db = SqliteSessionDatabase{getTestDatabaseFile()};
        auto db2 = SqliteSessionDatabase{getTestDatabaseFile()};
        auto updatedIndex = std::size_t{123456};
        std::ignore = db2.sessionUpdated.connect([&updatedIndex](std::size_t index) {
            updatedIndex = index;
        });

        auto insertResult = db.storeSession(Sessions::getTestSession3());
        REQUIRE_COMPARE_WITH_TIMEOUT(insertResult->getResult(), Result::Ok, 1s);
        insertResult = db.storeSession(Sessions::getTestSession4());
        REQUIRE_COMPARE_WITH_TIMEOUT(insertResult->getResult(), Result::Ok, 1s);

        auto lapData = Rapid::Common::LapData{};
        lapData.addSectorTimes({{"00:23:32.003"}, {"00:23:32.004"}, {"00:23:32.005"}});
        auto session1 = Sessions::getTestSession3();
        session1.addLap(lapData);

        insertResult = db.storeSession(session1);
        REQUIRE_COMPARE_WITH_TIMEOUT(insertResult->getResult(), Result::Ok, 1s);
        CHECK(insertResult->getResult() == Result::Ok);
        CHECK(updatedIndex == 0);
        REQUIRE(db.getSessionByIndex(0) == session1);
    }
}

TEST_CASE("The SqliteSessionDatabase shall gives the number of stored sessions and should return the correct session"
          "for that index.")
{
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
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
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    auto const session1 = Sessions::getTestSession3();
    auto const session2 = Sessions::getTestSession4();
    auto deletedIndex = std::size_t{123456};
    constexpr auto indexToDelete = 1;
    std::ignore = db.sessionDeleted.connect([&deletedIndex](std::size_t index) {
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
    auto db = SqliteSessionDatabase{getTestDatabaseFile()};
    auto const session1 = Sessions::getTestSession3();
    auto deletedIndex = std::size_t{123456};
    constexpr auto indexToDelete = 0;

    auto storeResult = db.storeSession(session1);
    storeResult->waitForFinished();
    CHECK(storeResult->getResult() == Result::Ok);
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
