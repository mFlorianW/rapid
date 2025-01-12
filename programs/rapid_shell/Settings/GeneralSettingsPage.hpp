// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>
#include <memory>

namespace Ui
{
class GeneralSettingsPage;
}

namespace Rapid::RapidShell::Settings
{

/**
 * Defines the content of the gernal settings pages in the global shell settings and
 * also handles the user input of changes in the global settings page.
 */
class GeneralSettingsPage : public QWidget
{
    Q_OBJECT
public:
    /**
     * Creates an instance of the GeneralSettingsPage
     */
    GeneralSettingsPage();

    /**
     * Default destructor
     */
    ~GeneralSettingsPage() override;

    /*
     * Disabled copy and move semantic
     */
    Q_DISABLE_COPY_MOVE(GeneralSettingsPage)

private:
    std::unique_ptr<Ui::GeneralSettingsPage> mGeneralSettings;
};
} // namespace Rapid::RapidShell::Settings
