// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DeviceModel.hpp"
#include <QHeaderView>
#include <QWidget>
#include <memory>

namespace Ui
{
class DevicePage;
}

namespace Rapid::RapidShell::Settings
{

/**
 * Defines the content for devices page in the global shell settings.
 */
class DevicePage : public QWidget
{
    Q_OBJECT
public:
    /**
     * Creates an Instance of the device settings pagage.
     */
    DevicePage(GlobalSettingsWriter* settingsWriter, Common::Qt::GlobalSettingsReader* settingsReader);

    /**
     * default destructor
     */
    ~DevicePage() override;

    /*
     * Disabled copy and move semantics
     */
    Q_DISABLE_COPY_MOVE(DevicePage)

    /**
     * Restores all settings on this page
     * @return true All settings are restored
     * @return false Failed to restore all settings
     */
    bool restore();

    /**
     * Stores all the changed settings of this page in the global settings
     * @return true All settings are stored.
     * @return false Failed to store the settings.
     */
    bool save();

private:
    DeviceModel mDeviceModel;
    QHeaderView mHeaderView{Qt::Horizontal};
    std::unique_ptr<Ui::DevicePage> mDevicePage;
};

} // namespace Rapid::RapidShell::Settings
