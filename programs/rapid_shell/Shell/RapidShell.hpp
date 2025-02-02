// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <ApplicationOverviewWidget.hpp>
#include <GlobalSettingsWindow.hpp>
#include <QMainWindow>
#include <QThread>
#include <SessionDatabaseIpcServer.hpp>
#include <common/qt/private/GlobalSettingsWriter.hpp>

namespace Ui
{
class MainWindow;
} // namespace Ui

namespace Rapid::RapidShell
{

class DatabaseBackend;
class RapidShell : public QMainWindow
{
    Q_OBJECT
public:
    RapidShell();
    ~RapidShell() override;

    Q_DISABLE_COPY_MOVE(RapidShell)

private:
    void setupDatabase() noexcept;
    std::unique_ptr<Ui::MainWindow> mMainWindow;
    Common::Qt::QSettingsBackend mSettingsBackend;
    Common::Qt::Private::GlobalSettingsWriter mSettings;
    std::unique_ptr<ProcessManager> mProcessManager;
    std::unique_ptr<Settings::GlobalSettingsWindow> mGlobalSettingsWindow;
    std::unique_ptr<ApplicationOverviewWidget> mApplicationOverviewWidget;
    QThread mDatabaseEndpoint;
    std::unique_ptr<DatabaseBackend> mDatabaseBackend;
};

} // namespace Rapid::RapidShell
