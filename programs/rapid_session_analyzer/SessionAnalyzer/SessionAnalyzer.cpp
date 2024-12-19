// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionAnalyzer.hpp"
#include "ui_SessionAnalyzer.h"
#include <QQuickWindow>
#include <spdlog/spdlog.h>

namespace Rapid::SessionAnalyzer
{

SessionAnalyzer::SessionAnalyzer()
    : QMainWindow{}
    , mMainWindow{std::make_unique<Ui::SessionAnalyzer>()}
    , mSessionDatabase{std::make_unique<Storage::Qt::SessionDatabaseIpcClient>()}
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
            auto sessoin = maybeSession.value();
            SPDLOG_INFO("{} {} {} selected for analyzing ...",
                        sessoin.getSessionDate().asString(),
                        sessoin.getSessionTime().asString(),
                        sessoin.getTrack().getTrackName());
        }
    });
}

} // namespace Rapid::SessionAnalyzer
