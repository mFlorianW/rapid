// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "common/qt/GlobalSettingsReader.hpp"
#include "common/qt/private/GlobalSettingsKeys.hpp"

namespace Rapid::Common::Qt
{

GlobalSettingsReader::GlobalSettingsReader(GlobalSettingsBackend* settingsBackend)
    : mSettingsBackend{settingsBackend}
{
}

GlobalSettingsReader::~GlobalSettingsReader() = default;

QString GlobalSettingsReader::getDbFilePath() const noexcept
{
    return mSettingsBackend->getValue(Private::DbFilePathKey).toString();
}

QList<DeviceSettings> GlobalSettingsReader::getDeviceSettings() const noexcept
{
    auto const deviceCount = mSettingsBackend->getValue(Private::DeviceSettingsSize);
    if (not deviceCount.isValid()) {
        return {};
    }
    auto devices = QList<DeviceSettings>{};
    devices.resize(deviceCount.toUInt());
    for (qsizetype i = 0; i < deviceCount.toUInt(); ++i) {
        auto const deviceName = mSettingsBackend->getValue(Private::DeviceSettingsName.toString().arg(i));
        if (not deviceName.isValid()) {
            return {};
        }
        devices[i].name = deviceName.toString();
        auto const deviceIp = mSettingsBackend->getValue(Private::DeviceSettingsIp.toString().arg(i));
        if (not deviceIp.isValid()) {
            return {};
        }
        auto ip = QHostAddress{deviceIp.toString()};
        if (ip.isNull()) {
            return {};
        }
        devices[i].ip = ip;
        auto const port = mSettingsBackend->getValue(Private::DeviceSettingsPort.toString().arg(i));
        if (not port.isValid() and not port.canConvert<quint16>()) {
            return {};
        }
        devices[i].port = port.toUInt();
        auto const defaultDevice = mSettingsBackend->getValue(Private::DeviceSettingsDef.toString().arg(i));
        if (not port.isValid() and not port.canConvert<bool>()) {
            return {};
        }
        devices[i].defaultDevice = defaultDevice.toBool();
    }
    return devices;
}

} // namespace Rapid::Common::Qt
