// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include "SqliteTrackDatabase.hpp"
#include <CompareHelper.hpp>
#include <SqliteDatabaseTestHelper.hpp>
#include <catch2/catch_all.hpp>
#include <chrono>

using namespace Rapid::Storage;
using namespace Rapid::TestHelper;
using namespace Rapid::TestHelper::SqliteDatabaseTestHelper;

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

TEST_CASE("The SqliteTrackDatabase shall delete a specific track.")
{
    auto trackDb = SqliteTrackDatabase{getTestDatabaseFile()};

    auto osl = Rapid::Common::TrackData{};
    osl.setTrackName("Oschersleben");
    osl.setStartline({52.0271, 11.2804});
    osl.setFinishline({52.0270889, 11.2803483});
    osl.setSections({{52.0298205, 11.2741851}, {52.0299681, 11.2772076}});

    auto const deleteResult = trackDb.deleteTrack(1);
    deleteResult->waitForFinished();
    REQUIRE(deleteResult->getResult() == Result::Ok);

    auto const tracks = trackDb.getTracks();
    REQUIRE_THAT(tracks, Catch::Matchers::UnorderedEquals(std::vector<Rapid::Common::TrackData>{osl}));
}

TEST_CASE("The SqliteTrackDatabase shall save a track")
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

    auto osl2 = Rapid::Common::TrackData{};
    osl2.setTrackName("Oschersleben2");
    osl2.setStartline({52, 11});
    osl2.setFinishline({52, 11});
    osl2.setSections({{52, 11}, {52, 11}});

    auto const saveResult = trackDb.saveTrack(osl2);
    REQUIRE_COMPARE_WITH_TIMEOUT(saveResult->getResult(), Result::Ok, std::chrono::seconds{1});

    auto const tracks = trackDb.getTracks();
    REQUIRE(tracks.size() == 3);
    REQUIRE_THAT(tracks, Catch::Matchers::UnorderedEquals(std::vector<Rapid::Common::TrackData>{osl, assen, osl2}));
}
