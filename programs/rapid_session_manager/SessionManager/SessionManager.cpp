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
    connect(mSessionManager->DeviceDownloadButton,
            &QPushButton::clicked,
            this,
            &SessionManager::onDownloadSessionRequest);

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
        mRestSessionManagement = std::make_unique<Workflow::Qt::RestSessionManagementWorkflow>(mRestClient.get());
        mRestSessionManagement->downloadAllSessionMetadata();
        mRestSessionMetadataProvider = mRestSessionManagement->getProvider();
        mRestSessionModel = std::make_unique<RestSessionModel>(*mRestSessionMetadataProvider);
        mSessionManager->DeviceSessionView->setModel(mRestSessionModel.get());
        std::ignore =
            mRestSessionManagement->sessionDownloadFinshed.connect(&SessionManager::onDownloadSessionRequestFinished,
                                                                   this);
    }
}

void SessionManager::onDownloadSessionRequest()
{
    auto selectedRows = mSessionManager->DeviceSessionView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        return;
    }
    auto const index = selectedRows.first().row();
    auto const maybeData = mRestSessionManagement->getSessionMetadata(index);
    if (not maybeData.has_value()) {
        return;
    }
    mRestSessionManagement->downloadSession(index);
    writeToDownloadLog(
        QString{"Start download: session %1 %2 %3"}.arg(QString::fromStdString(maybeData->getSessionDate().asString()),
                                                        QString::fromStdString(maybeData->getSessionTime().asString()),
                                                        QString::fromStdString(maybeData->getTrack().getTrackName())));
}

void SessionManager::onDownloadSessionRequestFinished(qsizetype index, Workflow::DownloadResult result)
{
    auto log = QString{"Error Download: Session for index %1 unknown error."}.arg(QString::number(index));
    if (result == Workflow::DownloadResult::Ok) {
        auto const maybeSession = mRestSessionManagement->getSession(index);
        if (maybeSession.has_value()) {
            log = QString{"Finish download: session %1 %2 %3"}.arg(
                QString::fromStdString(maybeSession->getSessionDate().asString()),
                QString::fromStdString(maybeSession->getSessionTime().asString()),
                QString::fromStdString(maybeSession->getTrack().getTrackName()));
            mSessionDatabase->storeSession(maybeSession.value());
        }
    } else {
        auto metadata = mRestSessionManagement->getSessionMetadata(index);
        if (metadata.has_value()) {
            log = QString{"Error download: Session %1 %2 %3"}.arg(
                QString::fromStdString(metadata->getSessionDate().asString()),
                QString::fromStdString(metadata->getSessionTime().asString()),
                QString::fromStdString(metadata->getTrack().getTrackName()));
        }
    }
    writeToDownloadLog(log);
}

void SessionManager::writeToDownloadLog(QString const& output)
{
    mSessionManager->DownloadLog->insertPlainText(output);
    mSessionManager->DownloadLog->insertPlainText("\n");
}

} // namespace Rapid::SessionManager
