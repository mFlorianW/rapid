// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <common/qt/SessionMetaDataListModel.hpp>
#include <testhelper/Tracks.hpp>

using namespace Rapid::Common::Qt;
using namespace Rapid::Common;
using namespace Rapid::TestHelper;

TEST_CASE("The SessionMetaDataListModel shall set the correct display roles")
{
    auto model = SessionMetaDataListModel{};
    REQUIRE(model.roleNames() == QHash<qint32, QByteArray>{{SessionMetaDataListModel::TrackName, "trackName"},
                                                           {SessionMetaDataListModel::Time, "time"},
                                                           {SessionMetaDataListModel::Date, "date"}});
}

TEST_CASE("The SessionMetaDataListModel shall give the correct data")
{
    auto model = SessionMetaDataListModel{};
    auto const sessionDate = Date{"01.01.1970"};
    auto const sessionTime = Timestamp{"01:00:00.000"};
    model.insertItem({Tracks::getTrack(), sessionDate, sessionTime});

    auto const trackName =
        model.data(model.index(0, 0), SessionMetaDataListModel::TrackName).value<QString>().toStdString();
    CHECK(trackName == Tracks::getTrack().getTrackName());
    auto const date = model.data(model.index(0, 0), SessionMetaDataListModel::Date).value<QString>().toStdString();
    CHECK(date == sessionDate.asString());
    auto const time = model.data(model.index(0, 0), SessionMetaDataListModel::Time).value<QString>().toStdString();
    CHECK(time == sessionTime.asString());
}
