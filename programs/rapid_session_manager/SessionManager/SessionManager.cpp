// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionManager.hpp"
#include "ui_SessionManager.h"

namespace Rapid::SessionManager
{

SessionManager::SessionManager()
    : mSessionManager{std::make_unique<Ui::SessionManager>()}
    , mSessionDatabase{std::make_unique<Storage::Qt::SessionDatabaseIpcClient>()}
    , mSettingsBackend{std::make_unique<Common::Qt::QSettingsBackend>()}
    , mSettingsReader{std::make_unique<Common::Qt::GlobalSettingsReader>(mSettingsBackend.get())}
    , mDeviceSettingsProvider{std::make_unique<Common::Qt::DeviceSettingsProvider>(*mSettingsReader)}
    , mDeviceSettingsModel{std::make_unique<DeviceSettingsModel>(*mDeviceSettingsProvider)}
{
    mSessionManager->setupUi(this);

    // Setup HostSessionTableView
    mSessionManager->HostSessionTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mSessionManager->HostSessionTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    mSessionManager->HostSessionTableView->horizontalHeader()->setStretchLastSection(true);

    connect(mSessionManager->DeleteHostSession, &QPushButton::clicked, this, &SessionManager::onDeleteHostSession);

    // Setup DeviceSessionTableView
    mSessionManager->DeviceSessionView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mSessionManager->DeviceSessionView->setSelectionMode(QAbstractItemView::SingleSelection);
    mSessionManager->DeviceSessionView->horizontalHeader()->setStretchLastSection(true);

    connect(mSessionManager->DeviceRefreshSessionView, &QPushButton::clicked, this, [this] {
        mRestSessionManagement->downloadAllSessionMetadata();
    });
    connect(mToolbarDeviceComboBox.get(), &QComboBox::activated, this, &SessionManager::onDeviceActivated);

    // Menubar
    connect(mSessionManager->actionQuit, &QAction::triggered, this, [] {
        QApplication::exit();
    });

    // Defer the model creation until the database is initialized
    connect(mSessionDatabase.get(), &Storage::Qt::SessionDatabaseIpcClient::initialized, this, [this] {
        mHostSessionMetaDataProvider = std::make_unique<Storage::Qt::SessionMetaDataProvider>(*mSessionDatabase);
        mHostSessionModel = std::make_unique<SessionModel>(*mHostSessionMetaDataProvider);
        mSessionManager->HostSessionTableView->setModel(mHostSessionModel.get());
    });

    //Setup Toolbar
    mToolbarDeviceComboBox->setModel(mDeviceSettingsModel.get());
    mSessionManager->toolBar->addWidget(mToolbarDeviceComboBox.get());
}

SessionManager::~SessionManager() = default;

void SessionManager::onDeleteHostSession()
{
    auto selectedRows = mSessionManager->HostSessionTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        return;
    }
    auto maybeSession = mHostSessionMetaDataProvider->getItem(selectedRows.first().row());
    if (not maybeSession.has_value()) {
        return;
    }
    mSessionDatabase->deleteSession(maybeSession->getId());
}

void SessionManager::onDeviceActivated(qsizetype index)
{
    auto const& device = mDeviceSettingsProvider->getItem(index);
    if (device.has_value()) {
        mRestClient = std::make_unique<Rest::QRestClient>();
        mRestClient->setServerAddress(device->ip.toString().toStdString());
        mRestClient->setServerPort(device->port);
        mRestSessionManagement = std::make_unique<Workflow::Qt::RestSessionManagementWorkflow>(*mRestClient);
        mRestSessionManagement->downloadAllSessionMetadata();
        mRestSessionMetadataProvider = mRestSessionManagement->getProvider();
        mRestSessionModel = std::make_unique<RestSessionModel>(*mRestSessionMetadataProvider);
        mSessionManager->DeviceSessionView->setModel(mRestSessionModel.get());
    }
}

} // namespace Rapid::SessionManager
