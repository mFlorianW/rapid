// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionSelectionDialog.hpp"
#include "ui_SessionSelectionDialog.h"
#include <spdlog/spdlog.h>

namespace Rapid::SessionAnalyzer
{

SessionSelectionDialog::SessionSelectionDialog(Storage::ISessionDatabase& sessionDatabase)
    : mSelectionDialogUi{std::make_unique<Ui::SessionSelectionDialog>()}
    , mSessionDatabase{sessionDatabase}
    , mSessionMetaDataProvider{std::make_unique<Storage::Qt::SessionMetaDataProvider>(mSessionDatabase)}
    , mSessionSelectionModel{std::make_unique<Common::Qt::GenericTableModel<Storage::Qt::SessionMetaDataProvider>>(
          *mSessionMetaDataProvider)}
{
    setModal(true);
    mSelectionDialogUi->setupUi(this);

    // Setup the HeaderView for the SessionSelectionModel
    mSessionSelectionModelHeaderView.setModel(mSessionSelectionModel.get());
    mSessionSelectionModelHeaderView.setStretchLastSection(true);

    // Setup the SessionSelectionModel
    mSelectionDialogUi->SessionTableView->setHorizontalHeader(&mSessionSelectionModelHeaderView);
    mSelectionDialogUi->SessionTableView->setModel(mSessionSelectionModel.get());
    mSelectionDialogUi->SessionTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mSelectionDialogUi->SessionTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(mSelectionDialogUi->CloseButton, &QPushButton::clicked, this, [this] {
        close();
    });

    connect(mSelectionDialogUi->SelectButton,
            &QPushButton::clicked,
            this,
            qOverload<>(&SessionSelectionDialog::onSessionSelected));
    connect(mSelectionDialogUi->SessionTableView,
            &QTableView::doubleClicked,
            this,
            qOverload<QModelIndex const&>(&SessionSelectionDialog::onSessionSelected));
}

SessionSelectionDialog::~SessionSelectionDialog() = default;

void SessionSelectionDialog::onSessionSelected(QModelIndex const& index)
{
    if (index.isValid()) {
        selectSession(index.row());
    }
}

void SessionSelectionDialog::onSessionSelected()
{
    auto selectedRows = mSelectionDialogUi->SessionTableView->selectionModel()->selectedRows();
    if (not selectedRows.isEmpty()) {
        selectSession(selectedRows.first().row());
    }
}

void SessionSelectionDialog::selectSession(qsizetype index)
{
    auto session = mSessionMetaDataProvider->getItem(index);
    if (session.has_value()) {
        Q_EMIT sessionSelected(session.value());
    }
    close();
}

} // namespace Rapid::SessionAnalyzer
