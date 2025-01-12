// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <DeviceModel.hpp>
#include <GlobalSettingsWriter.hpp>
#include <catch2/catch_all.hpp>
#include <common/qt/GlobalSettingsReader.hpp>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <common/qt/private/GlobalSettingsKeys.hpp>
#include <testhelper/qt/SettingsMemoryBackend.hpp>

using namespace Rapid::RapidShell::Settings;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;
using namespace Rapid::Common::Qt;

SCENARIO("The device list model shall provide the header data for the list view.")
{
    GIVEN("An initialized model")
    {
        auto backend = SettingsMemoryBackend{};
        auto writer = GlobalSettingsWriter{&backend};
        auto reader = GlobalSettingsReader{&backend};
        auto model = DeviceModel{&writer, &reader};
        WHEN("Requesting the header")
        {
            auto const headerDataCol0 = model.headerData(0, ::Qt::Orientation::Horizontal, ::Qt::DisplayRole);
            auto const headerDataCol1 = model.headerData(1, ::Qt::Orientation::Horizontal, ::Qt::DisplayRole);
            auto const headerDataCol2 = model.headerData(2, ::Qt::Orientation::Horizontal, ::Qt::DisplayRole);
            auto const headerDataCol3 = model.headerData(3, ::Qt::Orientation::Horizontal, ::Qt::DisplayRole);

            THEN("The correct header data is returned")
            {
                REQUIRE(headerDataCol0 == "Name");
                REQUIRE(headerDataCol1 == "IP-Address");
                REQUIRE(headerDataCol2 == "Port");
                REQUIRE(headerDataCol3 == "Default");
            }
        }
    }
}

SCENARIO("The device model shall add new devices to it's interal list")
{
    GIVEN("An initialized model")
    {
        auto backend = SettingsMemoryBackend{};
        auto writer = GlobalSettingsWriter{&backend};
        auto reader = GlobalSettingsReader{&backend};
        auto model = DeviceModel{&writer, &reader};
        WHEN("Adding a device")
        {
            REQUIRE(model.insertRows(0));
            THEN("The row count in the model should be increased and default data shall be returned.")
            {
                REQUIRE(model.rowCount(QModelIndex{}) == 1);
                REQUIRE(model.data(model.index(0, 0), ::Qt::DisplayRole).toString() == QStringLiteral("Lappy"));
                REQUIRE(model.data(model.index(0, 1), ::Qt::DisplayRole).toString() == QString("127.0.0.1"));
                REQUIRE(model.data(model.index(0, 2), ::Qt::DisplayRole).toString() == QString("80"));
                REQUIRE(model.data(model.index(0, 3), ::Qt::DisplayRole).toBool() == false);
            }
        }

        WHEN("Adding multple devices at once")
        {
            REQUIRE(model.insertRows(0, 5));
            THEN("The row count in the model should be increased and default data shall be returned.")
            {
                REQUIRE(model.rowCount(QModelIndex{}) == 5);
                for (int i = 0; i < 5; ++i) {
                    REQUIRE(model.data(model.index(i, 0), ::Qt::DisplayRole).toString() == QStringLiteral("Lappy"));
                    REQUIRE(model.data(model.index(i, 3), ::Qt::DisplayRole).toBool() == false);
                    REQUIRE(model.data(model.index(i, 2), ::Qt::DisplayRole).toString() == QString("80"));
                    REQUIRE(model.data(model.index(i, 1), ::Qt::DisplayRole).toString() == QString("127.0.0.1"));
                }
            }
        }
    }
}

SCENARIO("Remove existing device settings in the model")
{
    GIVEN("An initialized model")
    {
        auto backend = SettingsMemoryBackend{};
        auto writer = GlobalSettingsWriter{&backend};
        auto reader = GlobalSettingsReader{&backend};
        auto model = DeviceModel{&writer, &reader};
        WHEN("Removing one device settings in the model")
        {
            REQUIRE(model.insertRows(0));
            REQUIRE(model.rowCount(QModelIndex{}) == 1);
            REQUIRE(model.removeRows(0, 1));
            THEN("The row count should be changed and the removed row shall not be accessable")
            {
                REQUIRE(model.rowCount(QModelIndex{}) == 0);
                REQUIRE(model.data(model.index(0, 0), ::Qt::DisplayRole).isNull() == true);
                REQUIRE(model.data(model.index(0, 1), ::Qt::DisplayRole).isNull() == true);
                REQUIRE(model.data(model.index(0, 2), ::Qt::DisplayRole).isNull() == true);
                REQUIRE(model.data(model.index(0, 3), ::Qt::DisplayRole).isNull() == true);
            }
        }

        WHEN("Removing Multiple device settings")
        {
            REQUIRE(model.insertRows(0, 5));
            REQUIRE(model.rowCount(QModelIndex{}) == 5);
            REQUIRE(model.removeRows(0, 5));
            THEN("The row count should be changed to 0")
            {
                REQUIRE(model.rowCount(QModelIndex{}) == 0);
            }
        }
    }
}

SCENARIO("Editing Device settings in the model")
{
    GIVEN("A device model with some device settings")
    {
        auto backend = SettingsMemoryBackend{};
        auto writer = GlobalSettingsWriter{&backend};
        auto reader = GlobalSettingsReader{&backend};
        auto model = DeviceModel{&writer, &reader};
        REQUIRE(model.insertRows(0, 6));

        WHEN("Editing the device setting name for index 3")
        {
            auto const name = QStringLiteral("TestDevice1");
            auto const index = model.index(3, 0);
            REQUIRE(model.setData(index, name));
            THEN("The device setting name for the index should be changed")
            {
                REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == name);
            }
        }

        WHEN("Editing the device setting ip for index 2")
        {
            auto const ip = QHostAddress{"192.168.1.234"}.toString();
            auto const index = model.index(2, 1);
            REQUIRE(model.setData(index, ip));
            THEN("The device setting ip for the index should be changed.")
            {
                REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == ip);
            }
        }

        WHEN("Edit the device setting ip with invalid ip address for index 2")
        {
            auto const ip = QString{"aaaaa"};
            auto const index = model.index(2, 1);
            THEN("The device setting ip for the index should be changed.")
            {
                REQUIRE(model.setData(index, ip) == false);
            }
        }

        WHEN("Editing the device port for index 1")
        {
            auto const port = QString::number(123);
            auto const index = model.index(1, 2);
            REQUIRE(model.setData(index, port));
            THEN("The device setting port for the index should be changed.")
            {
                REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == port);
            }
        }

        WHEN("Editing the device port with invald input for index 1")
        {
            auto const port = QString{"aaaaa"};
            auto const index = model.index(1, 2);
            THEN("The change of the device settings should fail")
            {
                REQUIRE(model.setData(index, port) == false);
            }
        }
    }
}

SCENARIO("The device model shall store the device settings in the global settings")
{
    GIVEN("A device model with some device settings")
    {
        auto backend = SettingsMemoryBackend{};
        auto writer = GlobalSettingsWriter{&backend};
        auto reader = GlobalSettingsReader{&backend};
        auto model = DeviceModel{&writer, &reader};
        constexpr auto devices = int{2};
        REQUIRE(model.insertRows(0, devices));

        WHEN("The device model stores the device settings in the global settings")
        {
            REQUIRE(model.save() == true);
            THEN("The correct device settings shall be stored in the global settings")
            {
                auto const expectedSettings = DeviceSettings{.name = QStringLiteral("Lappy"),
                                                             .ip = QHostAddress{"127.0.0.1"},
                                                             .port = 80,
                                                             .defaultDevice = false};
                REQUIRE(backend.getValue(Private::DeviceSettingsSize).toInt() == devices);
                for (int i = 0; i < devices; i++) {
                    REQUIRE(backend.getValue(Private::DeviceSettingsName.toString().arg(i)).toString() == "Lappy");
                    REQUIRE(backend.getValue(Private::DeviceSettingsIp.toString().arg(i)).toString() == "127.0.0.1");
                    REQUIRE(backend.getValue(Private::DeviceSettingsPort.toString().arg(i)).toUInt() == 80);
                    REQUIRE(backend.getValue(Private::DeviceSettingsDef.toString().arg(i)).toBool() == false);
                }
            }
        }
    }
}

SCENARIO("Restore the original device settings")
{
    GIVEN("A device model with some device settings")
    {
        auto backend = SettingsMemoryBackend{};
        auto writer = GlobalSettingsWriter{&backend};
        auto reader = GlobalSettingsReader{&backend};
        auto model = DeviceModel{&writer, &reader};
        auto const expDeviceName = QStringLiteral("Lappy");
        constexpr auto devices = int{3};
        REQUIRE(model.insertRows(0, devices));
        model.save();

        WHEN("The device settings in the model are changed and restore of the original data is requested")
        {
            auto const index = model.index(1, 0);
            auto const deviceName = QStringLiteral("TestName");
            REQUIRE(model.setData(index, deviceName));
            REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == deviceName);
            REQUIRE(model.restore());

            THEN("The original data shall be restored restored")
            {
                REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == expDeviceName);
            }
        }

        WHEN("Device settings in the model are removed")
        {
            auto const index = model.index(2, 0);
            REQUIRE(model.removeRows(index.row()));
            REQUIRE(model.restore());
            THEN("The original data shall be restored restored")
            {
                REQUIRE(model.rowCount({}) == 3);
                REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == expDeviceName);
            }
        }

        WHEN("Device settings are added to the model")
        {
            REQUIRE(model.insertRows(model.rowCount({}), 3));
            REQUIRE(model.rowCount({}) == 6);
            REQUIRE(model.restore());
            THEN("On restore the old model row count should be returned")
            {
                REQUIRE(model.rowCount({}) == 3);
            }
        }

        WHEN("Device settings are stored don't restore the state for the save")
        {
            auto const index = model.index(1, 0);
            auto const deviceName = QStringLiteral("TestName");
            REQUIRE(model.setData(index, deviceName));
            REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == deviceName);
            REQUIRE(model.save());
            REQUIRE(model.restore());
            THEN("The new data shall be returned")
            {
                REQUIRE(model.data(index, ::Qt::DisplayRole).toString() == deviceName);
            }
        }
    }
}

SCENARIO("Load stored device settings")
{
    auto backend = SettingsMemoryBackend{};
    auto writer = GlobalSettingsWriter{&backend};
    auto reader = GlobalSettingsReader{&backend};
    auto const expDeviceName = QStringLiteral("StoredDevice1");
    auto const expHostAddress = QHostAddress{QHostAddress::LocalHost}.toString();
    auto const expPort = quint16{80};

    REQUIRE(backend.storeValue(Private::DeviceSettingsSize, 1));
    REQUIRE(backend.storeValue(Private::DeviceSettingsName.toString().arg(0), expDeviceName));
    REQUIRE(backend.storeValue(Private::DeviceSettingsIp.toString().arg(0), expHostAddress));
    REQUIRE(backend.storeValue(Private::DeviceSettingsPort.toString().arg(0), expPort));

    WHEN("The model is created the last stored device settings shall be loaded")
    {
        auto model = DeviceModel{&writer, &reader};
        THEN("The stored data is correctly loaded")
        {
            REQUIRE(model.rowCount({}) == 1);
            REQUIRE(model.data(model.index(0, 0), ::Qt::DisplayRole).toString() == expDeviceName);
            REQUIRE(model.data(model.index(0, 1), ::Qt::DisplayRole).toString() == expHostAddress);
            REQUIRE(model.data(model.index(0, 2), ::Qt::DisplayRole).toUInt() == expPort);
        }
    }
}
