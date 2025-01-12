// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "GlobalSettingsTypes.hpp"
#include <QString>
#include <common/qt/GlobalSettingsBackend.hpp>

namespace Rapid::Common::Qt
{

/**
 * Reads the global settings from the provided backend.
 */
class GlobalSettingsReader
{
public:
    /**
     * Creates an instance of the GlobalSettingsReader
     * @param settingsBackend The settings backend for the store and load operations
     */
    GlobalSettingsReader(GlobalSettingsBackend* settingsBackend);

    /**
     * Default destructor
     */
    ~GlobalSettingsReader();

    /**
     * Deleted copy constructor
     */
    GlobalSettingsReader(GlobalSettingsReader const&);

    /**
     * Deleted copy operator
     */
    GlobalSettingsReader& operator=(GlobalSettingsReader const&);

    /**
     * Deleted move constructor
     */
    GlobalSettingsReader(GlobalSettingsReader const&&) noexcept;

    /**
     * Deleted move operator
     */
    GlobalSettingsReader& operator=(GlobalSettingsReader&&) noexcept;

    /**
     * Gives the stored database file path.
     * If no database file is stored an empty string is returned.
     * @return Success: The stored database file path.
     *         Error: An emtpy string when not setting is found.
     */
    QString getDbFilePath() const noexcept;

    /**
     * Gives the stored device settings in the stored backend.
     * If no settings are stored the device settings will be empty.
     * @return Success: The stored device settings.
     *         Error or not found: An empty list is returned.
     */
    QList<DeviceSettings> getDeviceSettings() const noexcept;

private:
    GlobalSettingsBackend* mSettingsBackend;
};

} // namespace Rapid::Common::Qt
