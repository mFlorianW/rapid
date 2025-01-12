// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationConfigReader.hpp"
#include "FilePath.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::RapidShell;

SCENARIO("The application config reader shall read in a valid configuration")
{
    auto appCfgReader = ApplicationConfigReader{};

    WHEN("Reading a valid configuration")
    {
        auto const appConfig = appCfgReader.readConfig(QString{"%1/%2"}.arg(CONFIG_FILE_PATH, "ValidAppConfig.json"));
        THEN("The correct application config shall be returned")
        {
            auto const expectedConfig = ApplicationConfig{QString{"Testapp"},
                                                          QString{"/usr/bin/test"},
                                                          QString{"/usr/share/lappy/test/icon.png"},
                                                          {1, 0, 9}};
            REQUIRE(appConfig.has_value());
            REQUIRE(appConfig.value_or(expectedConfig) == expectedConfig);
        }
    }
}
