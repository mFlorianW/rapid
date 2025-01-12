// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef DEVICESETTINGSPROVIDER_HPP
#define DEVICESETTINGSPROVIDER_HPP

#include <common/qt/GlobalSettingsReader.hpp>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <common/qt/TableModelDataProvider.hpp>

namespace Rapid::Common::Qt
{
/**
 * @brief Provides the device settings for displaying in a @ref GenericTableModel
 *
 * @details The device settings are only read once on creation.
 *          The settings have the columns "Name", "IP" and "Port".
 */
class DeviceSettingsProvider : public TableModelDataProvider<DeviceSettings>
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(DeviceSettingsProvider)

    /**
     * Creates an instance of the @ref DeviceSettingsProvider.
     * @param settingsReader The settings that is used to read the device settings from
     */
    DeviceSettingsProvider(GlobalSettingsReader const& settingsReader);

    /**
     * Default destructor
     */
    ~DeviceSettingsProvider() override;
};

} // namespace Rapid::Common::Qt

#endif // DEVICESETTINGSPROVIDER_HPP
