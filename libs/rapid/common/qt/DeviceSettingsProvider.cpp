// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceSettingsProvider.hpp"

namespace Rapid::Common::Qt
{

DeviceSettingsProvider::DeviceSettingsProvider(GlobalSettingsReader const& settingsReader)
    : TableModelDataProvider<DeviceSettings>({tr("Name"), tr("IP-Address"), tr("Port")})
{
    auto const deviceSettings = settingsReader.getDeviceSettings();
    for (auto const& deviceSetting : deviceSettings) {
        addItem(deviceSetting);
    }

    setDataExtractor([](auto&& item, auto&& column, auto&& role) -> QVariant {
        if (role != ::Qt::DisplayRole) {
            return {};
        }

        switch (column) {
        case 0:
            return item.name;
        case 1:
            return item.ip.toString();
        case 2:
            return item.port;
        default:
            return QVariant{};
        }
    });
}

DeviceSettingsProvider::~DeviceSettingsProvider() = default;

} // namespace Rapid::Common::Qt
