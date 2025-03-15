// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <common/qt/LapListModel.hpp>
#include <testhelper/Sessions.hpp>

using namespace Rapid::Common;
using namespace Rapid::Common::Qt;
using namespace Rapid::TestHelper;

TEST_CASE("The LapListModel shall give the correct role names")
{
    auto const lapModel = LapListModel{};
    auto const roles = lapModel.roleNames();
    REQUIRE(roles == QHash<qint32, QByteArray>{{LapListModel::DisplayRole::Laptime, "laptime"}});
}

TEST_CASE("The LapListModel shall give the correct data for the roles")
{
    auto lapModel = LapListModel{};
    lapModel.insertItem(LapData{{"00:01:23.123"}});

    SECTION("Valid access")
    {
        auto const laptime = lapModel.data(lapModel.index(0), LapListModel::Laptime).toString();
        REQUIRE(laptime == QStringLiteral("00:01:23.123"));
    }

    SECTION("Invalid access with wrong index")
    {
        auto const laptime = lapModel.data(lapModel.index(2), LapListModel::Laptime).toString();
        REQUIRE(laptime == QString(""));
    }

    SECTION("Invalid access with unsupported DisplayRole")
    {
        constexpr auto unsupportedRole = 123456;
        auto const laptime = lapModel.data(lapModel.index(2), unsupportedRole).toString();
        REQUIRE(laptime == QString(""));
    }
}

TEST_CASE("Create a LapListModel from list of laps")
{
    auto model = LapListModel{Sessions::getTestSession3().getLaps()};
    constexpr auto expectedSize = 2U;
    REQUIRE(model.rowCount({}) == expectedSize);
}
