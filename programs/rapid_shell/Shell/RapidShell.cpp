// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidShell.hpp"
#include "DatabaseFile.hpp"
#include "LoggingCategories.hpp"
#include "ui_MainWindow.h"
#include <LoggingCategories.hpp>
#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QQmlContext>
#include <QStandardPaths>
#include <common/qt/GlobalSettingsReader.hpp>
#include <spdlog/spdlog.h>
#include <storage/SqliteSessionDatabase.hpp>
#include <system/qt/EventLoopIntegration.hpp>

namespace Rapid::RapidShell
{

class DatabaseBackend : public QObject
{
    Q_OBJECT
public:
    DatabaseBackend() = default;
    ~DatabaseBackend() override = default;
    Q_DISABLE_COPY_MOVE(DatabaseBackend);

public Q_SLOTS:
    void exeucteDatabaseBackend()
    {
        if (not Rapid::System::Qt::EventLoopIntegration::makeEventLoopIntegration()) {
            SPDLOG_CRITICAL("Failed to creat event loop integration don't start database endpoint");
            return;
        }
        auto settingsBackend = Common::Qt::QSettingsBackend{};
        auto dbFile = Common::Qt::GlobalSettingsReader{&settingsBackend}.getDbFilePath();
        auto sessionDatabase = std::make_unique<Rapid::Storage::SqliteSessionDatabase>(dbFile.toStdString());
        auto sessionDatabaseIpcServer = std::make_unique<Storage::SessionDatabaseIpcServer>(*sessionDatabase);
        QEventLoop{}.exec();
    }
};

RapidShell::RapidShell()
    : mMainWindow{std::make_unique<Ui::MainWindow>()}
    , mSettings{&mSettingsBackend}
    , mProcessManager{std::make_unique<ProcessManager>()}
    , mGlobalSettingsWindow{std::make_unique<Settings::GlobalSettingsWindow>()}
    , mApplicationOverviewWidget{std::make_unique<ApplicationOverviewWidget>(mProcessManager.get())}
    , mDatabaseBackend{std::make_unique<DatabaseBackend>()}
{
    setupDatabase();
    mDatabaseBackend->moveToThread(&mDatabaseEndpoint);
    connect(&mDatabaseEndpoint, &QThread::started, mDatabaseBackend.get(), &DatabaseBackend::exeucteDatabaseBackend);
    mDatabaseEndpoint.start();

    mMainWindow->setupUi(this);
    mMainWindow->centralwidget->layout()->addWidget(mApplicationOverviewWidget.get());

    connect(mMainWindow->actionQuit, &QAction::triggered, this, []() {
        QApplication::exit();
    });

    connect(mMainWindow->actionPreferences, &QAction::triggered, this, [this]() {
        mGlobalSettingsWindow->show();
    });
}

RapidShell::~RapidShell()
{
    if (mDatabaseEndpoint.isRunning()) {
        mDatabaseEndpoint.quit();
        mDatabaseEndpoint.wait();
    }
}

void RapidShell::setupDatabase() noexcept
{
    auto const dbLocation =
        QString{"%1/%2"}.arg(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::GenericDataLocation),
                             QStringLiteral("rapid"));
    auto const dbFile = QString{"%1%2"}.arg(dbLocation, QStringLiteral("/rapid.db"));
    if (not QFile::exists(dbFile)) {
        qCInfo(rapid_shell) << "No database found! Copy empty default database to" << dbLocation;
        auto dbDir = QDir{};
        if (not dbDir.exists(dbLocation)) {
            qCInfo(rapid_shell) << "Database location doesn't exists." << dbLocation;
            if (not dbDir.mkdir(dbLocation)) {
                qCCritical(rapid_shell) << "Failed to create database location." << dbFile;
            } else {
                qCInfo(rapid_shell) << "Database location created't exists." << dbLocation;
            }
        } else {
            qCInfo(rapid_shell) << "Database location exists." << dbLocation;
        }

        if (not QFile::copy(DATABASE_FILE, dbFile)) {
            qCCritical(rapid_shell) << "Failed to copy empty database to:" << dbFile;
        } else {
            qCInfo(rapid_shell) << "Copied empty database to:" << dbFile;
        }
    }

    if (not mSettings.storeDatabaseFilePath(dbFile)) {
        qCCritical(rapid_shell) << "Failed to store database file parameter in global settings.";
    }
}

} // namespace Rapid::RapidShell

#include "RapidShell.moc"
