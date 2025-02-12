// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <common/qt/DeviceSettingsListModel.hpp>

using namespace Rapid::Common::Qt;

TEST_CASE("The DeviceSettingsListModel shall give the correct role names")
{
    auto model = DeviceSettingsListModel{};
    REQUIRE(model.roleNames() ==
            QHash<int, QByteArray>{{::Qt::DisplayRole, "display"}, {DeviceSettingsListModel::Laptimer, "laptimer"}});
}

TEST_CASE("The DeviceSettingsListModel shall give the correct data when requested")
{
    auto model = DeviceSettingsListModel{};
    auto expectedDevice = DeviceSettings{.name = "Device1", .ip = QHostAddress{QHostAddress::LocalHost}, .port = 10};
    REQUIRE(model.insertItem(expectedDevice) == 0);

    SECTION("Request data for valid index")
    {
        CHECK(model.data(model.index(0), ::Qt::DisplayRole).value<QString>() == QStringLiteral("Device1"));
        CHECK(model.data(model.index(0), DeviceSettingsListModel::Laptimer).value<DeviceSettings>() == expectedDevice);
    }

    SECTION("Request data for invalid index")
    {
        CHECK(model.data(model.index(1), ::Qt::DisplayRole) == QVariant{});
        CHECK(model.data(model.index(1), DeviceSettingsListModel::Laptimer) == QVariant{});
    }
}
