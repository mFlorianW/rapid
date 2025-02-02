// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QSettings>
#include <QVariant>
#include <common/qt/GlobalSettingsBackend.hpp>
#include <common/qt/GlobalSettingsTypes.hpp>

namespace Rapid::Common::Qt::Private
{

/**
 * Write only interfacce for the global settings of the Shell
 */
class GlobalSettingsWriter
{
public:
    /**
     * Constructs a @ref Rapid::RapidShell::Settings instance
     * @param settingsBackend The settings that execute the concret store operations
     *                        The Settings doesn't take the ownership of the pointer so the caller must
     *                        guarantee that the object is valid as long the Settings class exists.
     */
    GlobalSettingsWriter(Common::Qt::GlobalSettingsBackend* settingsBackend);

    /**
     * Default destructor
     */
    ~GlobalSettingsWriter();

    /**
     * Deleted copy operator
     */
    GlobalSettingsWriter& operator=(GlobalSettingsWriter const&) = delete;

    /**
     * Deleted copy constructor
     */
    GlobalSettingsWriter(GlobalSettingsWriter const&) = delete;

    /**
     * Deleted move operator
     */
    GlobalSettingsWriter& operator=(GlobalSettingsWriter&&) noexcept = delete;

    /**
     * Deleted move constructor
     */
    GlobalSettingsWriter(GlobalSettingsWriter const&&) noexcept = delete;

    /**
     * Stores the database file path
     * @param dbFilePath The new database file path that shall be stored.
     * @return True successful stored
     *         false Failed to store
     */
    [[nodiscard]] bool storeDatabaseFilePath(QString const& dbFilePath) noexcept;

    /**
     * Stores the device settings
     * @param deviceSettings The list of device settings that shall be stored.
     * @return true the successful stored
     * @return false Failed to store
     */
    [[nodiscard]] bool storeDeviceSettings(QList<Common::Qt::DeviceSettings> const& deviceSettings) noexcept;

private:
    Common::Qt::GlobalSettingsBackend* mSettingsBackend;
};

} // namespace Rapid::Common::Qt::Private
