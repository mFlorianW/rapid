// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceManagement.hpp"
#include <common/qt/private/GlobalSettingsWriter.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::Common::Qt;

namespace Rapid::Workflow::Qt
{

DeviceManagement::DeviceManagement(Common::Qt::GlobalSettingsBackend* settingsBackend)
    : IDeviceManagement{}
    , mSettingsBackend{settingsBackend}
    , mGlobalSettingsReader{std::make_unique<Common::Qt::GlobalSettingsReader>(settingsBackend)}
    , mGlobalSettingsWriter{std::make_unique<Common::Qt::Private::GlobalSettingsWriter>(settingsBackend)}
    , mModel{std::make_unique<DeviceSettingsListModel>()}
{
    auto const devices = mGlobalSettingsReader->getDeviceSettings();
    if (devices.isEmpty()) {
        auto const defaultDevice =
            DeviceSettings{.name = QStringLiteral("Rapid"), .ip = QHostAddress{"192.168.1.1"}, .port = 27018};
        if (not store(defaultDevice)) {
            SPDLOG_ERROR("Failed to store default laptimer.");
        } else {
            SPDLOG_INFO("No laptimer device found. Created default laptimer device.");
        }
    } else {
        for (auto const& device : devices) {
            mModel->insertItem(device);
        }
        SPDLOG_INFO("Laptimer configuration successful loaded.");
    }
}

DeviceManagement::~DeviceManagement() = default;

QAbstractItemModel const* const DeviceManagement::getModel() const noexcept
{
    return mModel.get();
}

bool DeviceManagement::store(Common::Qt::DeviceSettings const& device) noexcept
{
    auto devices = mGlobalSettingsReader->getDeviceSettings();
    if (devices.contains(device)) {
        SPDLOG_INFO("Don't store laptimer configuration. Reason: Already stored");
        return false;
    }
    devices.append(device);
    if (mGlobalSettingsWriter->storeDeviceSettings(devices)) {
        mModel->insertItem(device);
        SPDLOG_INFO("Successful stored laptimer configuration: {}", device);
        return true;
    }
    SPDLOG_ERROR("Failed to store laptimer configuration. Unknwon error");
    return false;
}

bool DeviceManagement::remove(Rapid::Common::Qt::DeviceSettings const& device) noexcept
{
    auto devices = mGlobalSettingsReader->getDeviceSettings();
    devices.removeAll(device);
    if (not mGlobalSettingsWriter->storeDeviceSettings(devices)) {
        SPDLOG_ERROR("Failed to store device configuration. Unknwon error");
        return false;
    }
    if (mModel->removeItem(device)) {
        SPDLOG_INFO("Successful removed laptimer configuration: {}", device);
        return true;
    }
    SPDLOG_ERROR("Failed to store laptimer configuration. Unknwon error");
    return false;
}

bool DeviceManagement::update(Rapid::Common::Qt::DeviceSettings const& oldDevice,
                              Rapid::Common::Qt::DeviceSettings const& newDevice) noexcept
{
    auto devices = mGlobalSettingsReader->getDeviceSettings();
    bool updated = false;
    for (auto& device : devices) {
        if (device == oldDevice) {
            device = newDevice;
            updated = true;
        }
    }
    if (not updated) {
        SPDLOG_ERROR("Failed to updated device configuration. No Laptimer found");
        return false;
    }
    if (not mGlobalSettingsWriter->storeDeviceSettings(devices)) {
        SPDLOG_ERROR("Failed to store updated device configuration. Unknwon error");
        return false;
    }
    if (not mModel->updateItem(oldDevice, newDevice)) {
        SPDLOG_ERROR("Failed to store updated device configuration in the model. Unknwon error");
        return false;
    }
    SPDLOG_INFO("Successful update laptimer configuration: {} with {}", oldDevice, newDevice);
    return true;
}

Rapid::Common::Qt::DeviceSettings DeviceManagement::create(QString const& name,
                                                           QString const& ip,
                                                           QString port,
                                                           bool enabled) noexcept
{
    auto device = DeviceSettings{.name = name, .port = static_cast<quint16>(port.toUInt()), .defaultDevice = enabled};
    device.setIpAddress(ip);
    return device;
}

} // namespace Rapid::Workflow::Qt
