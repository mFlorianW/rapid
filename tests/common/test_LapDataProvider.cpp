// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <common/qt/LapDataProvider.hpp>
#include <ranges>
#include <testhelper/Tracks.hpp>

using namespace Rapid::Common::Qt;
using namespace Rapid::Common;
using namespace Rapid::TestHelper::Tracks;

struct TestFixture
{
    std::vector<LapData> getLapsWithSectors()
    {
        auto laps = std::vector<LapData>{};
        laps.reserve(2);
        laps.emplace_back(getLap());
        laps.emplace_back(getLap());
        return laps;
    }

    LapData getLap()
    {
        Timestamp sectorTime;
        sectorTime.setFractionalOfSecond(144);
        sectorTime.setSecond(25);

        auto lap = LapData{};
        lap.addSectorTime(sectorTime);
        lap.addSectorTime(sectorTime);
        lap.addSectorTime(sectorTime);
        return lap;
    }
};

TEST_CASE_METHOD(TestFixture, "The LapDataProvider columns are defined by the TrackData sections")
{
    SECTION("Track with multiple secionts")
    {
        auto provider = LapDataProvider{getOscherslebenTrack()};
        auto columns = provider.getColumnCount();
        CHECK(columns == 4);
        auto columnNames = provider.getColumnNames();
        REQUIRE(columnNames == QStringList{QStringLiteral("Lap time"),
                                           QStringLiteral("Section0"),
                                           QStringLiteral("Section1"),
                                           QStringLiteral("Section2")});
    }

    SECTION("Track with only finishline")
    {

        auto provider = LapDataProvider{getTrackWithoutSector()};
        auto columns = provider.getColumnCount();
        REQUIRE(columns == 1);
        auto columnNames = provider.getColumnNames();
        REQUIRE(columnNames == QStringList{QStringLiteral("Lap time")});
    }
}

TEST_CASE_METHOD(TestFixture, "The LapDataProvider shall provide the lap time and section times")
{
    auto provider = LapDataProvider{getOscherslebenTrack(), getLapsWithSectors()};
    for (auto row : std::views::iota(0, 2)) {
        auto lapTime = provider.data(row, 0, ::Qt::DisplayRole);
        auto section0 = provider.data(row, 1, ::Qt::DisplayRole);
        auto section1 = provider.data(row, 2, ::Qt::DisplayRole);
        auto section2 = provider.data(row, 3, ::Qt::DisplayRole);
        CHECK(lapTime.toString() == QStringLiteral("00:01:15.432"));
        CHECK(section0.toString() == QStringLiteral("00:00:25.144"));
        CHECK(section1.toString() == QStringLiteral("00:00:25.144"));
        CHECK(section2.toString() == QStringLiteral("00:00:25.144"));
    }
}

TEST_CASE_METHOD(TestFixture, "The LapDataProvider row count are defined by the LapData")
{
    auto provider = LapDataProvider{getOscherslebenTrack(), getLapsWithSectors()};
    auto rowCount = provider.getRowCount();
    REQUIRE(rowCount == 2);
}
