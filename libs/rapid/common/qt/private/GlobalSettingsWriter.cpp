// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GlobalSettingsWriter.hpp"
#include <common/qt/private/GlobalSettingsKeys.hpp>

namespace Rapid::Common::Qt::Private
{

GlobalSettingsWriter::GlobalSettingsWriter(Common::Qt::GlobalSettingsBackend* settingsBackend)
    : mSettingsBackend{settingsBackend}
{
}

GlobalSettingsWriter::~GlobalSettingsWriter() = default;

bool GlobalSettingsWriter::storeDatabaseFilePath(QString const& dbFilePath) noexcept
{
    return mSettingsBackend->storeValue(Common::Qt::Private::DbFilePathKey, dbFilePath);
}

bool GlobalSettingsWriter::storeDeviceSettings(QList<Common::Qt::DeviceSettings> const& deviceSettings) noexcept
{
    if (deviceSettings.isEmpty()) {
        return true;
    }
    if (not mSettingsBackend->storeValue(Common::Qt::Private::DeviceSettingsSize, deviceSettings.size())) {
        return false;
    }
    for (qsizetype i = 0; i < deviceSettings.size(); ++i) {
        if (not mSettingsBackend->storeValue(Common::Qt::Private::DeviceSettingsName.toString().arg(i),
                                             deviceSettings[i].name) or
            not mSettingsBackend->storeValue(Common::Qt::Private::DeviceSettingsIp.toString().arg(i),
                                             deviceSettings[i].ip.toString()) or
            not mSettingsBackend->storeValue(Common::Qt::Private::DeviceSettingsPort.toString().arg(i),
                                             deviceSettings[i].port) or
            not mSettingsBackend->storeValue(Common::Qt::Private::DeviceSettingsDef.toString().arg(i),
                                             deviceSettings[i].defaultDevice)) {
            return false;
        }
    }
    return true;
}

} // namespace Rapid::Common::Qt::Private
