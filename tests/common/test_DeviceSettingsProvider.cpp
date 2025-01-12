// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <common/qt/DeviceSettingsProvider.hpp>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <common/qt/private/GlobalSettingsKeys.hpp>
#include <list>
#include <testhelper/qt/SettingsBackendMock.hpp>

using namespace Rapid::Common;
using namespace Rapid::Common::Qt;
using namespace Rapid::TestHelper;
using namespace trompeloeil;

namespace
{

struct TestFixture
{
    DeviceSettings const device1 =
        DeviceSettings{.name = QStringLiteral("Device1"), .ip = QHostAddress{"127.0.0.1"}, .port = 27018};
    DeviceSettings const device2 =
        DeviceSettings{.name = QStringLiteral("Device2"), .ip = QHostAddress{"127.0.0.1"}, .port = 27018};
    std::list<DeviceSettings> mDevices{device1, device2};

    SettingsBackendMock settingsBackendMock;
    GlobalSettingsReader settingsReader{&settingsBackendMock};

    std::unique_ptr<DeviceSettingsProvider> dsp;

    TestFixture()
    {
        // Stored amount of device settings
        REQUIRE_CALL(settingsBackendMock, getValue(_)).WITH(_1 == Private::DeviceSettingsSize).RETURN(2);

        // // Calls Device1
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings0Name"))
            .LR_RETURN(device1.name);
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings0Ip"))
            .LR_RETURN(device1.ip.toString());
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings0Port"))
            .LR_RETURN(device1.port);
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings0Default"))
            .LR_RETURN(device1.defaultDevice);
        // Calls Device2
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings1Name"))
            .LR_RETURN(device2.name);
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings1Ip"))
            .LR_RETURN(device2.ip.toString());
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings1Port"))
            .LR_RETURN(device2.port);
        REQUIRE_CALL(settingsBackendMock, getValue(_))
            .WITH(_1 == QStringLiteral("deviceSettings1Default"))
            .LR_RETURN(device2.defaultDevice);

        dsp = std::make_unique<DeviceSettingsProvider>(settingsReader);
    }
};

} // namespace

TEST_CASE_METHOD(TestFixture, "The DeviceSettingsProvider shall set the correct columnames ")
{
    REQUIRE(dsp->getColumnCount() == 3);
    REQUIRE(dsp->getColumnNames() ==
            QStringList{QStringLiteral("Name"), QStringLiteral("IP-Address"), QStringLiteral("Port")});
}

TEST_CASE_METHOD(TestFixture, "The DeviceSettingsProvider shall request the device settings in the settings")
{
    REQUIRE(dsp->getRowCount() == 2);
    REQUIRE(dsp->getItem(0) == device1);
    REQUIRE(dsp->getItem(1) == device2);
}

TEST_CASE_METHOD(TestFixture, "The DeviceSettingsProvider shall give the correct data for the UI")
{
    CHECK(dsp->data(0, 0, ::Qt::DisplayRole).toString() == device1.name);
    CHECK(dsp->data(0, 1, ::Qt::DisplayRole).toString() == device1.ip.toString());
    CHECK(dsp->data(0, 2, ::Qt::DisplayRole).toString() == QString::number(device1.port));

    CHECK(dsp->data(1, 0, ::Qt::DisplayRole).toString() == device2.name);
    CHECK(dsp->data(1, 1, ::Qt::DisplayRole).toString() == device2.ip.toString());
    CHECK(dsp->data(1, 2, ::Qt::DisplayRole).toString() == QString::number(device2.port));
}
