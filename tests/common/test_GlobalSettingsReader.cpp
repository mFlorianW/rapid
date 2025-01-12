// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QHostAddress>
#include <catch2/catch_all.hpp>
#include <common/qt/GlobalSettingsReader.hpp>
#include <common/qt/private/GlobalSettingsKeys.hpp>
#include <testhelper/qt/SettingsMemoryBackend.hpp>

using namespace Rapid::Common;
using namespace Rapid::Common::Qt;
using namespace Rapid::TestHelper;

SCENARIO("The Settings shall load the settings database file path from the provided backend.")
{
    GIVEN("Initialized Settings instance")
    {
        auto settingsBackend = SettingsMemoryBackend();
        REQUIRE(settingsBackend.storeValue(QStringLiteral("dbFilePath"), QStringLiteral("/test/1.db")));
        auto settings = GlobalSettingsReader{&settingsBackend};
        WHEN("The database file path is requested")
        {
            auto const dbFile = settings.getDbFilePath();
            THEN("The correct path shall be returned")
            {
                REQUIRE(dbFile == "/test/1.db");
            }
        }
    }
}

SCENARIO("The Settings shall load the device settings from the provided backend.")
{
    GIVEN("Initialized Settings instance")
    {
        auto settingsBackend = SettingsMemoryBackend();

        constexpr auto devices = int{2};

        REQUIRE(settingsBackend.storeValue(Private::DeviceSettingsSize, devices));
        for (int i = 0; i < devices; ++i) {
            REQUIRE(settingsBackend.storeValue(Private::DeviceSettingsName.toString().arg(i),
                                               QStringLiteral("Lappy%1").arg(i)));
            REQUIRE(settingsBackend.storeValue(Private::DeviceSettingsIp.toString().arg(i),
                                               QHostAddress{QHostAddress::LocalHost}.toString()));
            REQUIRE(settingsBackend.storeValue(Private::DeviceSettingsPort.toString().arg(i), 80));
            REQUIRE(settingsBackend.storeValue(Private::DeviceSettingsDef.toString().arg(i), false));
        }

        auto settings = GlobalSettingsReader{&settingsBackend};
        WHEN("The database file path is requested")
        {
            auto const deviceSettings = settings.getDeviceSettings();
            THEN("The correct path shall be returned")
            {
                REQUIRE(deviceSettings.size() == devices);
                for (int i = 0; i < devices; ++i) {
                    REQUIRE(deviceSettings.at(i).name == QStringLiteral("Lappy%1").arg(i));
                    REQUIRE(deviceSettings.at(i).ip == QHostAddress{QHostAddress::LocalHost});
                    REQUIRE(deviceSettings.at(i).port == 80);
                    REQUIRE(deviceSettings.at(i).defaultDevice == false);
                }
            }
        }
    }
}
