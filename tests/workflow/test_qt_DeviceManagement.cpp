// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <common/qt/private/GlobalSettingsKeys.hpp>
#include <testhelper/qt/SettingsBackendMock.hpp>
#include <workflow/qt/DeviceManagement.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::TestHelper;
using namespace trompeloeil;
using namespace Rapid::Common::Qt;
using Expectation = std::unique_ptr<trompeloeil::expectation>;

namespace
{

struct TestFixtue
{
    SettingsBackendMock settingsBackendMock;
    std::vector<Expectation> mExpectations;
    DeviceSettings defaulDevice{.name = "Rapid",
                                .ip = QHostAddress{"192.168.1.1"},
                                .port = 27018,
                                .defaultDevice = false};

    DeviceManagement createWithOneDevice()
    {
        auto const index = QString::number(0);
        mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                       .WITH(_1 == Private::DeviceSettingsSize.toString())
                                       .RETURN(1));
        mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                       .WITH(_1 == Private::DeviceSettingsIp.toString().arg(index))
                                       .LR_RETURN(QVariant::fromValue(defaulDevice.ip.toString())));
        mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                       .WITH(_1 == Private::DeviceSettingsName.toString().arg(index))
                                       .LR_RETURN(defaulDevice.name));
        mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                       .WITH(_1 == Private::DeviceSettingsPort.toString().arg(index))
                                       .LR_RETURN(defaulDevice.port));
        mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                       .WITH(_1 == Private::DeviceSettingsDef.toString().arg(index))
                                       .LR_RETURN(defaulDevice.defaultDevice));
        mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true));
        return DeviceManagement{std::addressof(settingsBackendMock)};
    }
};

} // namespace

TEST_CASE_METHOD(TestFixtue,
                 "The DeviceManagement shall give a Model for displaying the devices",
                 "[DEVICE_MANAGEMENT_MODEL]")
{
    ALLOW_CALL(settingsBackendMock, getValue(_)).RETURN(QVariant{});
    ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true);
    DeviceManagement deviceManagement{std::addressof(settingsBackendMock)};
    SECTION("Give the model a valid model")
    {
        auto* model = deviceManagement.getModel();
        REQUIRE(model != nullptr);
    }

    SECTION("Property gives a valid model")
    {
        REQUIRE(deviceManagement.property("model").value<QAbstractItemModel*>() != nullptr);
    }
}

TEST_CASE_METHOD(TestFixtue,
                 "The DeviceManagement shall create a default device when no device is stored"
                 "[DEVICE_MANAGEMENT_DEFAULT_DEVICE]")
{

    ALLOW_CALL(settingsBackendMock, getValue(_)).RETURN(QVariant{});
    ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true);
    auto const device = Rapid::Common::Qt::DeviceSettings{.name = QStringLiteral("Rapid"),
                                                          .ip = QHostAddress{QStringLiteral("192.168.1.1")},
                                                          .port = 27015,
                                                          .defaultDevice = false};
    auto deviceManagement = DeviceManagement{std::addressof(settingsBackendMock)};
    SECTION("Create Default Device")
    {
        auto* model = deviceManagement.getModel();
        CHECK(model->rowCount() == 1);
        auto laptimerSettings =
            model->data(model->index(0, 0), DeviceSettingsListModel::Laptimer).value<DeviceSettings>();
        CHECK(laptimerSettings.name == QStringLiteral("Rapid"));
        CHECK(laptimerSettings.ip == QHostAddress("192.168.1.1"));
        CHECK(laptimerSettings.port == 27018);
    }
}

TEST_CASE_METHOD(TestFixtue, "The DeviceMangement shall store devices (laptimer)", "[DEVICE_MANAGEMENT_STORE]")
{
    auto const device = Rapid::Common::Qt::DeviceSettings{.name = QStringLiteral("rapid"),
                                                          .ip = QHostAddress{QStringLiteral("192.168.1.2")},
                                                          .port = 27015,
                                                          .defaultDevice = false};
    SECTION("Store device")
    {
        auto deviceManagement = createWithOneDevice();

        // The expected storage calls
        REQUIRE_CALL(settingsBackendMock, storeValue(_, _)).WITH(_2 == QVariant{2}).RETURN(true);
        REQUIRE_CALL(settingsBackendMock, storeValue(_, _)).WITH(_2 == QVariant{device.name}).RETURN(true);
        REQUIRE_CALL(settingsBackendMock, storeValue(_, _))
            .WITH(_2 == QVariant::fromValue<QString>(device.ip.toString()))
            .RETURN(true);
        REQUIRE_CALL(settingsBackendMock, storeValue(_, _)).WITH(_2 == QVariant{device.port}).RETURN(true);
        REQUIRE_CALL(settingsBackendMock, storeValue(_, _)).WITH(_2 == QVariant{device.defaultDevice}).RETURN(true);

        // The storage calls
        REQUIRE(deviceManagement.store(device));
    }

    SECTION("Stored device is added to the model")
    {
        auto deviceManagement = createWithOneDevice();
        REQUIRE(deviceManagement.store(device));
        CHECK(deviceManagement.getModel()->rowCount() == 2);
    }

    SECTION("Store opertaion failed")
    {
        auto deviceManagement = createWithOneDevice();
        ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(false);
        REQUIRE_FALSE(deviceManagement.store(device));
        REQUIRE(deviceManagement.getModel()->rowCount() == 1);
    }

    SECTION("Don't store the same device twice")
    {
        auto deviceManagement = createWithOneDevice();
        REQUIRE_FALSE(deviceManagement.store(defaulDevice));
    }
}

TEST_CASE_METHOD(TestFixtue, "The DeviceMangement shall delete devices (laptimer)", "[DEVICE_MANAGEMENT_DELETE]")
{
    ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true);
    SECTION("Delete existing device")
    {
        auto deviceManagement = createWithOneDevice();
        REQUIRE(deviceManagement.remove(defaulDevice));
        REQUIRE(deviceManagement.getModel()->rowCount() == 0);
    }

    SECTION("Delete not existing device")
    {
        auto deviceManagement = createWithOneDevice();
        REQUIRE_FALSE(deviceManagement.remove(DeviceSettings{}));
        REQUIRE(deviceManagement.getModel()->rowCount() == 1);
    }
}

TEST_CASE_METHOD(TestFixtue, "The DeviceSettings shall update devices (laptimer)", "[DEVICE_MANAGEMENT_UPDATE]")
{
    ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true);
    SECTION("Update existing device")
    {
        auto deviceManagement = createWithOneDevice();
        auto updatedDevice = defaulDevice;
        REQUIRE(deviceManagement.update(defaulDevice, updatedDevice));
    }

    SECTION("Update an invalid device")
    {
        auto deviceManagement = createWithOneDevice();
        REQUIRE_FALSE(deviceManagement.update(DeviceSettings{}, DeviceSettings{}));
    }
}
