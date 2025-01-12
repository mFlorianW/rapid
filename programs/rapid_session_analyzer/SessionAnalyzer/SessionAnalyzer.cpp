// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionAnalyzer.hpp"
#include "ui_SessionAnalyzer.h"
#include <QQuickWindow>
#include <spdlog/spdlog.h>
#include <workflow/qt/SessionAnalyzeWorkflow.hpp>

namespace Rapid::SessionAnalyzer
{

SessionAnalyzer::SessionAnalyzer()
    : QMainWindow{}
    , mMainWindow{std::make_unique<Ui::SessionAnalyzer>()}
    , mSessionDatabase{std::make_unique<Storage::Qt::SessionDatabaseIpcClient>()}
    , mSessionAnalyzerWorkflow{std::make_unique<Workflow::Qt::SessionAnalyzeWorkflow>()}
{
    mMainWindow->setupUi(this);
    connect(mMainWindow->actionQuit, &QAction::triggered, this, []() {
        QApplication::exit();
    });

    connect(mMainWindow->actionOpen_Session, &QAction::triggered, this, [this]() {
        if (mSessionSelectionDialog != nullptr) {
            mSessionSelectionDialog->show();
        }
    });

    // Defer the selection dialog until the database is initialized
    connect(mSessionDatabase.get(), &Storage::Qt::SessionDatabaseIpcClient::initialized, this, [this] {
        mSessionSelectionDialog = std::make_unique<SessionSelectionDialog>(*mSessionDatabase);
        connect(mSessionSelectionDialog.get(),
                &SessionSelectionDialog::sessionSelected,
                this,
                &SessionAnalyzer::onSessionSelected);
    });

    // Configure LapDataView
    mMainWindow->LapDataView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mMainWindow->LapDataView->setSelectionMode(QAbstractItemView::SingleSelection);
    mMainWindow->LapDataView->horizontalHeader()->setStretchLastSection(true);
}

SessionAnalyzer::~SessionAnalyzer() = default;

void SessionAnalyzer::onSessionSelected(Rapid::Common::SessionMetaData const& selectedSession)
{
    auto result = mSessionDatabase->getSessionByMetadataAsync(selectedSession);
    mSessionRequests.insert({result.get(), result});
    std::ignore = result->done.connect([this](auto* result) {
        if (mSessionRequests.count(result) == 0) {
            return;
        }
        auto resultPtr = mSessionRequests.at(result);
        auto maybeSession = resultPtr->getResultValue();
        if (maybeSession.has_value()) {
            auto session = maybeSession.value();
            mSessionAnalyzerWorkflow->setSession(session);
            mMainWindow->LapDataView->setModel(mSessionAnalyzerWorkflow->lapModel.get().get());
            SPDLOG_INFO("{} {} {} selected for analyzing ...",
                        session.getSessionDate().asString(),
                        session.getSessionTime().asString(),
                        session.getTrack().getTrackName());
        }
    });
}

} // namespace Rapid::SessionAnalyzer
