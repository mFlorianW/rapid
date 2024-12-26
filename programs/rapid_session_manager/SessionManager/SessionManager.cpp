// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionManager.hpp"
#include "ui_SessionManager.h"

namespace Rapid::SessionManager
{

SessionManager::SessionManager()
    : mSessionManager{std::make_unique<Ui::SessionManager>()}
    , mSessionDatabase{std::make_unique<Storage::Qt::SessionDatabaseIpcClient>()}
{
    mSessionManager->setupUi(this);

    // Setup HostSessionTableView
    mSessionManager->HostSessionTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mSessionManager->HostSessionTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    mSessionManager->HostSessionTableView->horizontalHeader()->setStretchLastSection(true);

    connect(mSessionManager->actionQuit, &QAction::triggered, this, [] {
        QApplication::exit();
    });

    // Defer the model creation until the database is initialized
    connect(mSessionDatabase.get(), &Storage::Qt::SessionDatabaseIpcClient::initialized, this, [this] {
        mHostSessionMetaDataProvider = std::make_unique<Storage::Qt::SessionMetaDataProvider>(*mSessionDatabase);
        mHostSessionModel = std::make_unique<SessionModel>(*mHostSessionMetaDataProvider);
        mSessionManager->HostSessionTableView->setModel(mHostSessionModel.get());
    });

    connect(mSessionManager->DeleteHostSession, &QPushButton::clicked, this, &SessionManager::onDeleteHostSession);
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

} // namespace Rapid::SessionManager
