// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <GlobalSettingsWriter.hpp>
#include <catch2/catch_all.hpp>
#include <common/qt/GlobalSettingsBackend.hpp>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <common/qt/private/GlobalSettingsKeys.hpp>
#include <testhelper/qt/SettingsMemoryBackend.hpp>

using namespace Rapid::RapidShell::Settings;
using namespace Rapid::Common;
using namespace Rapid::Common::Qt;
using namespace Rapid::TestHelper;

SCENARIO("The Settings shall store the settings database file path.")
{
    GIVEN("Initialized Settings instance")
    {
        auto settingsBackend = SettingsMemoryBackend();
        auto settings = GlobalSettingsWriter{&settingsBackend};
        WHEN("The databease file path is stored.")
        {
            auto const expDbFilePath = QStringLiteral("/test/1.db");
            REQUIRE(settings.storeDatabaseFilePath(expDbFilePath));
            THEN("The stored database file path shall be stored correctly in the backend")
            {
                auto dbFilePath = settingsBackend.getValue(Private::DbFilePathKey).toString();
                REQUIRE(dbFilePath == expDbFilePath);
            }
        }
    }
}

SCENARIO("The GlobalSettingsWriter shall store the device settings")
{
    GIVEN("Initialized Settings instance")
    {
        auto settingsBackend = SettingsMemoryBackend();
        auto settings = GlobalSettingsWriter{&settingsBackend};

        WHEN("Saving an empty device setting list.")
        {
            THEN("Shall return true")
            {
                auto const deviceList = QList<DeviceSettings>{};
                REQUIRE(settings.storeDeviceSettings(deviceList));
            }
        }

        WHEN("The a list of device settings is stored")
        {
            auto deviceSettings = DeviceSettings{.name = "0", .ip = QHostAddress{"127.0.0.1"}, .port = 80};
            auto deviceSettingList = QList<DeviceSettings>{deviceSettings, deviceSettings};
            REQUIRE(settings.storeDeviceSettings(deviceSettingList));
            THEN("The correct device settings are stored in the backend.")
            {
                REQUIRE(settingsBackend.getValue("deviceSettingsSize").toInt() == 2);
                for (int i = 0; i < 2; ++i) {
                    REQUIRE(settingsBackend.getValue(Private::DeviceSettingsName.toString().arg(i)).toString() == "0");
                    REQUIRE(settingsBackend.getValue(Private::DeviceSettingsIp.toString().arg(i)).toString() ==
                            "127.0.0.1");
                    REQUIRE(settingsBackend.getValue(Private::DeviceSettingsPort.toString().arg(i)).toString() == "80");
                    REQUIRE(settingsBackend.getValue(Private::DeviceSettingsDef.toString().arg(i)).toBool() == false);
                }
            }
        }
    }
}
