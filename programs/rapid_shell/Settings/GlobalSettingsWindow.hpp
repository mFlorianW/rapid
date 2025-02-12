// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DevicePage.hpp"
#include "GeneralSettingsPage.hpp"
#include <QDialog>
#include <QStackedLayout>
#include <QtGlobal>
#include <common/qt/GlobalSettingsBackend.hpp>
#include <common/qt/GlobalSettingsReader.hpp>
#include <common/qt/private/GlobalSettingsWriter.hpp>
#include <memory>

namespace Ui
{
class GlobalSettingsWindow;
class GeneralSettings;
class DeviceSettings; // NOLINT(bugprone-forward-declaration-namespace)
} // namespace Ui

namespace Rapid::RapidShell::Settings
{

/**
 * Defines the layout and the content of the global settings in the shell
 * also handles the user input for the navigation in the settings.
 */
class GlobalSettingsWindow : public QDialog
{
    Q_OBJECT
public:
    /**
     * Creates an Instance of the GlobalSettingsWindow
     */
    GlobalSettingsWindow();

    /**
     * Default destructor
     */
    ~GlobalSettingsWindow() override;

    /*
     * Disabled copy and move operators
     */
    Q_DISABLE_COPY_MOVE(GlobalSettingsWindow);

private:
    Common::Qt::QSettingsBackend mSettingsBackend = Common::Qt::QSettingsBackend{};
    Common::Qt::Private::GlobalSettingsWriter mSettingsWriter =
        Common::Qt::Private::GlobalSettingsWriter{&mSettingsBackend};
    Common::Qt::GlobalSettingsReader mSettingsReader = Common::Qt::GlobalSettingsReader{&mSettingsBackend};
    std::unique_ptr<Ui::GlobalSettingsWindow> mGlobalSettingsWindow;
    std::unique_ptr<QStackedLayout> mSettingsPages;
    std::unique_ptr<GeneralSettingsPage> mGeneralSettingsPage;
    std::unique_ptr<DevicePage> mDevicePage;
};

} // namespace Rapid::RapidShell::Settings
