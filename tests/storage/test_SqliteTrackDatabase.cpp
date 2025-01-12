// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "storage/SqliteTrackDatabase.hpp"
#include <catch2/catch_all.hpp>
#include <chrono>
#include <storage/private/Connection.hpp>
#include <testhelper/CompareHelper.hpp>
#include <testhelper/SqliteDatabaseTestHelper.hpp>

using namespace Rapid::Storage;
using namespace Rapid::TestHelper;
using namespace Rapid::TestHelper::SqliteDatabaseTestHelper;
using namespace std::chrono_literals;

namespace
{

class SqliteTrackDatabaseEventListener : public SqliteDatabaseTestEventlistener
{
    using SqliteDatabaseTestEventlistener::SqliteDatabaseTestEventlistener;

    [[nodiscard]] std::string getCleanDbFileName() const noexcept override
    {
        static auto dbFile = std::string{"test_trackmanagement.db"};
        return dbFile;
    }
};

std::vector<Rapid::Common::TrackData> getDefaultTracks()
{
    auto osl = Rapid::Common::TrackData{};
    osl.setTrackName("Oschersleben");
    osl.setStartline({52.0271, 11.2804});
    osl.setFinishline({52.0270889, 11.2803483});
    osl.setSections({{52.0298205, 11.2741851}, {52.0299681, 11.2772076}});

    auto assen = Rapid::Common::TrackData{};
    assen.setTrackName("Assen");
    assen.setFinishline({52.962324, 6.524115});
    assen.setSections({{52.959453, 6.525305}, {52.955628, 6.512773}});
    return {std::move(osl), std::move(assen)};
}

} // namespace

CATCH_REGISTER_LISTENER(SqliteTrackDatabaseEventListener);

TEST_CASE("The SqliteDatabaseTrackDatabase shall return the number of stored tracks.")
{
    auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
    REQUIRE(trackDb.getTrackCount() == 2);
}

TEST_CASE("The SqliteDatabaseTrackDatabase shall return all stored tracks.")
{
    auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};

    auto osl = Rapid::Common::TrackData{};
    osl.setTrackName("Oschersleben");
    osl.setStartline({52.0271, 11.2804});
    osl.setFinishline({52.0270889, 11.2803483});
    osl.setSections({{52.0298205, 11.2741851}, {52.0299681, 11.2772076}});

    auto assen = Rapid::Common::TrackData{};
    assen.setTrackName("Assen");
    assen.setFinishline({52.962324, 6.524115});
    assen.setSections({{52.959453, 6.525305}, {52.955628, 6.512773}});

    auto const tracks = trackDb.getTracks();

    REQUIRE_THAT(tracks, Catch::Matchers::UnorderedEquals(std::vector<Rapid::Common::TrackData>{osl, assen}));
}

TEST_CASE("The SqliteTrackDatabase shall delete a specific track and notify about changes")
{
    auto osl = Rapid::Common::TrackData{};
    osl.setTrackName("Oschersleben");
    osl.setStartline({52.0271, 11.2804});
    osl.setFinishline({52.0270889, 11.2803483});
    osl.setSections({{52.0298205, 11.2741851}, {52.0299681, 11.2772076}});

    SECTION("Delete track")
    {
        auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
        auto const deleteResult = trackDb.deleteTrack(1);
        REQUIRE_COMPARE_WITH_TIMEOUT(deleteResult->getResult(), Result::Ok, 1s);
        auto const tracks = trackDb.getTracks();
        REQUIRE_THAT(tracks, Catch::Matchers::UnorderedEquals(std::vector<Rapid::Common::TrackData>{osl}));
    }

    SECTION("Delete tack signal is emitted")
    {
        auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
        auto trackDeleted = false;
        auto trackDeletedIndex = std::size_t{12345};
        std::ignore = trackDb.trackDeleted.connect([&trackDeleted, &trackDeletedIndex](std::size_t index) {
            trackDeleted = true;
            trackDeletedIndex = index;
        });
        auto const deleteResult = trackDb.deleteTrack(1);
        REQUIRE_COMPARE_WITH_TIMEOUT(deleteResult->getResult(), Result::Ok, 1s);
        auto const tracks = trackDb.getTracks();
        CHECK(trackDeleted);
        REQUIRE(trackDeletedIndex == 1);
    }

    SECTION("Delete tack signal is emitted from other instances also")
    {
        auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
        auto trackDb2 = SqliteTrackDatabase{getTestDatabaseFile()};
        auto trackDeleted = false;
        auto trackDeletedIndex = std::size_t{12345};
        std::ignore = trackDb2.trackDeleted.connect([&trackDeleted, &trackDeletedIndex](std::size_t index) {
            trackDeleted = true;
            trackDeletedIndex = index;
        });
        auto const deleteResult = trackDb.deleteTrack(1);
        REQUIRE_COMPARE_WITH_TIMEOUT(deleteResult->getResult(), Result::Ok, 1s);
        auto const tracks = trackDb.getTracks();
        CHECK(trackDeleted);
        REQUIRE(trackDeletedIndex == 1);
    }
}

TEST_CASE("The SqliteTrackDatabase shall save a track")
{
    auto trackAdded = false;
    auto expectedIndex = std ::size_t{2};
    auto addedIndex = std::size_t{12123213};
    auto trackAddedHandler = [&addedIndex, &trackAdded](std::size_t index) {
        trackAdded = true;
        addedIndex = index;
    };

    auto osl = Rapid::Common::TrackData{};
    osl.setTrackName("Oschersleben");
    osl.setStartline({52.0271, 11.2804});
    osl.setFinishline({52.0270889, 11.2803483});
    osl.setSections({{52.0298205, 11.2741851}, {52.0299681, 11.2772076}});

    auto assen = Rapid::Common::TrackData{};
    assen.setTrackName("Assen");
    assen.setFinishline({52.962324, 6.524115});
    assen.setSections({{52.959453, 6.525305}, {52.955628, 6.512773}});

    auto osl2 = Rapid::Common::TrackData{};
    osl2.setTrackName("Oschersleben2");
    osl2.setStartline({52, 11});
    osl2.setFinishline({52, 11});
    osl2.setSections({{52, 11}, {52, 11}});

    SECTION("The SqliteTrackDatabase store a track and emits signal for track added")
    {
        auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
        std::ignore = trackDb.trackAdded.connect(trackAddedHandler);
        auto const saveResult = trackDb.saveTrack(osl2);
        REQUIRE_COMPARE_WITH_TIMEOUT(saveResult->getResult(), Result::Ok, std::chrono::seconds{1});
        auto const tracks = trackDb.getTracks();
        CHECK(tracks.size() == 3);
        CHECK_THAT(tracks, Catch::Matchers::UnorderedEquals(std::vector<Rapid::Common::TrackData>{osl, assen, osl2}));
        CHECK(trackAdded);
        REQUIRE(addedIndex == expectedIndex);
    }

    SECTION("The track added signal must be emit from all track database instances")
    {
        auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
        auto trackDb2 = SqliteTrackDatabase{getTestDatabaseFile()};
        std::ignore = trackDb2.trackAdded.connect(trackAddedHandler);
        auto const saveResult = trackDb.saveTrack(osl2);
        REQUIRE_COMPARE_WITH_TIMEOUT(saveResult->getResult(), Result::Ok, std::chrono::seconds{1});
        CHECK(trackAdded);
        REQUIRE(addedIndex == expectedIndex);
        ;
    }

    SECTION("The track added signal after multiple changes on the database")
    {
        auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
        auto const saveResult = trackDb.saveTrack(osl2);
        REQUIRE_COMPARE_WITH_TIMEOUT(saveResult->getResult(), Result::Ok, std::chrono::seconds{1});
        auto const deleteResult = trackDb.deleteTrack(2);
        REQUIRE_COMPARE_WITH_TIMEOUT(deleteResult->getResult(), Result::Ok, std::chrono::seconds{1});
        std::ignore = trackDb.trackAdded.connect(trackAddedHandler);
        auto const saveResult2 = trackDb.saveTrack(osl2);
        REQUIRE_COMPARE_WITH_TIMEOUT(saveResult2->getResult(), Result::Ok, std::chrono::seconds{1});
        CHECK(trackAdded);
        REQUIRE(addedIndex == expectedIndex);
    }
}

TEST_CASE("Delete all tracks in the SqliteTrackDatabase")
{
    auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};

    auto const saveResult = trackDb.deleteAllTracks();
    REQUIRE_COMPARE_WITH_TIMEOUT(saveResult->getResult(), Result::Ok, std::chrono::seconds{1});

    auto const tracks = trackDb.getTracks();
    REQUIRE(tracks.size() == 0);

    auto resultHandler = [](void*, int columns, char**, char**) -> int {
        REQUIRE(columns == 0);
        return SQLITE_OK;
    };

    // Make sure that the tables are realy cleared.
    auto* dbCon = Private::Connection::connection(getTestDatabaseFile())->getRawHandle();
    REQUIRE(sqlite3_exec(dbCon, "SELECT * FROM Sektor", resultHandler, nullptr, nullptr) == SQLITE_OK);
    REQUIRE(sqlite3_exec(dbCon, "SELECT * FROM Track", resultHandler, nullptr, nullptr) == SQLITE_OK);
    REQUIRE(sqlite3_exec(dbCon, "SELECT * FROM Position", resultHandler, nullptr, nullptr) == SQLITE_OK);
}

TEST_CASE("Read the track count with an asynchronous")
{
    auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
    auto const result = trackDb.getTrackCountAsync();

    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE_COMPARE_WITH_TIMEOUT(result->getResultValue().has_value(), true, std::chrono::milliseconds{10});
    REQUIRE(result->getResultValue().value());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("Get all tracks asynchronous")
{
    auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};
    auto const result = trackDb.getTracksAsync();

    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE_COMPARE_WITH_TIMEOUT(result->getResultValue().has_value(), true, std::chrono::milliseconds{10});
    REQUIRE(result->getResultValue().value() == getDefaultTracks());
    // NOLINTEND(bugprone-unchecked-optional-access)
}
