// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QSignalSpy>
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

    DeviceManagement createDeviceMangement(std::size_t devices = 1)
    {
        if (devices < 1) {
            FAIL("Must be created with at least one device");
        }
        mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                       .WITH(_1 == Private::DeviceSettingsSize.toString())
                                       .RETURN(QVariant::fromValue(devices)));
        for (std::size_t i = 0; i < devices; ++i) {
            auto const index = QString::number(i);
            auto settings = createDeviceSettings(i);
            mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                           .WITH(_1 == Private::DeviceSettingsIp.toString().arg(index))
                                           .RETURN(QVariant::fromValue(settings.ip.toString())));
            mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                           .WITH(_1 == Private::DeviceSettingsName.toString().arg(index))
                                           .RETURN(settings.name));
            mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                           .WITH(_1 == Private::DeviceSettingsPort.toString().arg(index))
                                           .RETURN(settings.port));
            mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, getValue(_))
                                           .WITH(_1 == Private::DeviceSettingsDef.toString().arg(index))
                                           .RETURN(settings.defaultDevice));
            mExpectations.emplace_back(NAMED_ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true));
        }
        return {std::addressof(settingsBackendMock)};
    }

    DeviceSettings createDeviceSettings(std::size_t deviceIndex)
    {
        return {.name = QString{"Rapid%1"}.arg(deviceIndex),
                .ip = QHostAddress{"192.168.0.1"},
                .port = 27018,
                .defaultDevice = false};
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
        CHECK(laptimerSettings.defaultDevice == true);
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
        auto deviceManagement = createDeviceMangement();

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
        auto deviceManagement = createDeviceMangement();
        REQUIRE(deviceManagement.store(device));
        CHECK(deviceManagement.getModel()->rowCount() == 2);
    }

    SECTION("Store opertaion failed")
    {
        auto deviceManagement = createDeviceMangement();
        ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(false);
        REQUIRE_FALSE(deviceManagement.store(device));
        REQUIRE(deviceManagement.getModel()->rowCount() == 1);
    }

    SECTION("Don't store the same device twice")
    {
        auto deviceManagement = createDeviceMangement();
        REQUIRE_FALSE(deviceManagement.store(createDeviceSettings(0)));
    }
}

TEST_CASE_METHOD(TestFixtue, "The DeviceMangement shall delete devices (laptimer)", "[DEVICE_MANAGEMENT_DELETE]")
{
    ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true);
    SECTION("Delete existing device")
    {
        auto deviceManagement = createDeviceMangement();
        REQUIRE(deviceManagement.remove(createDeviceSettings(0)));
        REQUIRE(deviceManagement.getModel()->rowCount() == 0);
    }

    SECTION("Delete not existing device")
    {
        auto deviceManagement = createDeviceMangement();
        REQUIRE_FALSE(deviceManagement.remove(DeviceSettings{}));
        REQUIRE(deviceManagement.getModel()->rowCount() == 1);
    }
}

TEST_CASE_METHOD(TestFixtue, "The DeviceMangement shall update devices (laptimer)", "[DEVICE_MANAGEMENT_UPDATE]")
{
    ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true);
    SECTION("Update existing device")
    {
        auto deviceManagement = createDeviceMangement();
        auto updatedDevice = createDeviceSettings(1);
        REQUIRE(deviceManagement.update(createDeviceSettings(0), updatedDevice));
    }

    SECTION("Update an invalid device")
    {
        auto deviceManagement = createDeviceMangement();
        REQUIRE_FALSE(deviceManagement.update(DeviceSettings{}, DeviceSettings{}));
    }
}

TEST_CASE_METHOD(TestFixtue,
                 "The DeviceMangement shall only one enabled device (laptimer)",
                 "[DEVICE_MANAGEMENT_ENABLED]")
{
    ALLOW_CALL(settingsBackendMock, storeValue(_, _)).RETURN(true);
    SECTION("Enable device")
    {
        auto deviceManagement = createDeviceMangement();
        auto device = createDeviceSettings(0);
        auto activeLaptimerSpy = QSignalSpy{&deviceManagement, &DeviceManagement::activeLaptimerChanged};
        REQUIRE(deviceManagement.enable(device));
        auto model = deviceManagement.getModel();
        CHECK(model->data(model->index(0, 0), DeviceSettingsListModel::Laptimer).value<DeviceSettings>().defaultDevice);
        CHECK(deviceManagement.property("activeLaptimer").value<DeviceSettings>() == device);
        CHECK(activeLaptimerSpy.size() == 1);
    }

    SECTION("No device enabled, default is automatically active")
    {
        auto deviceManagement = createDeviceMangement();
        CHECK(deviceManagement.property("activeLaptimer").value<DeviceSettings>() == createDeviceSettings(0));
    }

    SECTION("Active device deleted first device is new active device")
    {
        // TODO: This test needs some refactoring
        // The remove call fails because the mock always returns 2 devices
        // This should be cleaned up when the device settings are stored in a repository which is easier to mock.
        auto deviceManagement = createDeviceMangement(2);
        auto enabledDevice = createDeviceSettings(0);
        auto* const model = deviceManagement.getModel();
        REQUIRE(deviceManagement.enable(enabledDevice));
        enabledDevice = model->data(model->index(1, 0), DeviceSettingsListModel::Laptimer).value<DeviceSettings>();
        deviceManagement.remove(enabledDevice);
        auto const isDeviceEnabled =
            model->data(model->index(0, 0), DeviceSettingsListModel::Laptimer).value<DeviceSettings>().defaultDevice;
        REQUIRE(isDeviceEnabled);
    }
}
