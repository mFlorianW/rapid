// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationModel.hpp"
#include "FilePath.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::RapidShell;

SCENARIO("The application shall load all applications from a list of folders and return the configs")
{
    auto mdl = ApplicationModel{{CONFIG_APP_PATH}};
    auto const expectedApps = QVector<ApplicationConfig>{{QStringLiteral("Testapp1"),
                                                          QStringLiteral("/usr/bin/test"),
                                                          QStringLiteral("/usr/share/lappy/test/icon.png"),
                                                          ApplicationVersion::fromString("1.2.3")},
                                                         {QStringLiteral("Testapp2"),
                                                          QStringLiteral("/usr/bin/test"),
                                                          QStringLiteral("/usr/share/lappy/test/icon.png"),
                                                          ApplicationVersion::fromString("1.2.3")}};
    WHEN("The applications are loaded")
    {
        mdl.loadApplications();
        THEN("The correct applications configs shall be returned")
        {
            auto const apps = mdl.getApplications();
            REQUIRE(apps.contains(expectedApps[0]));
            REQUIRE(apps.contains(expectedApps[1]));
        }
    }
}
