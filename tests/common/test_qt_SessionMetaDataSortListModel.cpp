// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <common/qt/SessionMetaDataListModel.hpp>
#include <common/qt/SessionMetaDataSortListModel.hpp>
#include <testhelper/Sessions.hpp>
#include <testhelper/Tracks.hpp>

using namespace Rapid::Common;
using namespace Rapid::Common::Qt;
using namespace Rapid::TestHelper;

TEST_CASE("The SessionMetaDataSortListModel shall sort sessions decending")
{
    auto sourceModel = SessionMetaDataListModel{};
    auto sortModel = SessionMetaDataSortListModel{};
    sortModel.setSourceModel(&sourceModel);

    SECTION("Sort on time")
    {
        auto const s1 = SessionMetaData{Tracks::getOscherslebenTrack(), Date{"01.01.1970"}, Timestamp{"01:00:00.234"}};
        auto const s2 = SessionMetaData{Tracks::getOscherslebenTrack(), Date{"01.01.1970"}, Timestamp{"00:00:00.234"}};

        // two sessions meta data in the wrong order
        sourceModel.insertItem(s2);
        sourceModel.insertItem(s1);

        REQUIRE(sortModel.data(sortModel.index(0, 0), SessionMetaDataListModel::Time).toString().toStdString() ==
                s1.getSessionTime().asString());
        REQUIRE(sortModel.data(sortModel.index(1, 0), SessionMetaDataListModel::Time).toString().toStdString() ==
                s2.getSessionTime().asString());
    }

    SECTION("Sort on Date")
    {
        auto const s1 = SessionMetaData{Tracks::getOscherslebenTrack(), Date{"01.01.1970"}, Timestamp{"01:00:00.234"}};
        auto const s2 = SessionMetaData{Tracks::getOscherslebenTrack(), Date{"02.01.1970"}, Timestamp{"00:00:00.234"}};

        // two sessions meta data in the wrong order
        sourceModel.insertItem(s2);
        sourceModel.insertItem(s1);

        REQUIRE(sortModel.data(sortModel.index(0, 0), SessionMetaDataListModel::Date).toString().toStdString() ==
                s2.getSessionDate().asString());
        REQUIRE(sortModel.data(sortModel.index(1, 0), SessionMetaDataListModel::Date).toString().toStdString() ==
                s1.getSessionDate().asString());
    }
}
