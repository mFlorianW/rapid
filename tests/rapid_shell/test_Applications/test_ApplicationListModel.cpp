// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationListModel.hpp"
#include "FilePath.hpp"
#include <QAbstractItemModelTester>
#include <catch2/catch_all.hpp>

using namespace Rapid::RapidShell;

SCENARIO("The ApplicationListModel shall return the correct values for the UI")
{
    auto appModel = ApplicationModel{{CONFIG_APP_PATH}};
    appModel.loadApplications();
    auto listModel = ApplicationListModel{appModel};
    auto modelTester = QAbstractItemModelTester{&listModel};
    WHEN("The values for the UI are requested")
    {
        auto const appName = listModel.data(listModel.index(0), ApplicationListModel::AppName).toString();
        auto const iconUrl = listModel.data(listModel.index(0), ApplicationListModel::IconUrl).toString();
        auto const exe = listModel.data(listModel.index(0), ApplicationListModel::ExePath).toString();
        auto const count = listModel.rowCount(QModelIndex{});
        THEN("The correct values shall be returned.")
        {
            REQUIRE(count == 2);
            REQUIRE(appName == QStringLiteral("Testapp1"));
            REQUIRE(iconUrl == QStringLiteral("file:/usr/share/lappy/test/icon.png"));
            REQUIRE(exe == QStringLiteral("/usr/bin/test"));
        }
    }
}
