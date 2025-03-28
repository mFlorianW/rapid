// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionPageModel.hpp"
#include <spdlog/spdlog.h>

using namespace Rapid::Workflow::Qt;

namespace Rapid::Android
{

SessionPageModel::SessionPageModel(std::unique_ptr<Storage::ISessionDatabase> sessionDb)
    : mSessionDb{std::move(sessionDb)}
    , mLocalSessionMgmt{mSessionDb.get()}
{
    mLapListModelChangedConnection = mSessionAnalyzeWorkflow.lapListModel.valueChanged().connect([this] {
        Q_EMIT lapListModelChanged();
    });
    mSessionMetaSortModel.setSourceModel(mLocalSessionMgmt.getSessionMetaDataListModel());
}

SessionPageModel::~SessionPageModel() = default;

Rapid::Common::Qt::SessionMetaDataSortListModel* SessionPageModel::getSessionListModel() noexcept
{
    return &mSessionMetaSortModel;
}

Rapid::Common::Qt::LapListModel* SessionPageModel::getLapListModel() noexcept
{
    return mSessionAnalyzeWorkflow.lapListModel.get().get();
}

void SessionPageModel::analyzeSession(quint32 sessionIndex)
{
    auto const index =
        mSessionMetaSortModel.mapToSource(mSessionMetaSortModel.index(static_cast<int>(sessionIndex), 0));
    auto session = mLocalSessionMgmt.getSessionMetaDataListModel()->getElement(index.row());
    if (session.has_value()) {
        mDbQuery = mSessionDb->getSessionByIndexAsync(session.value()->getId());
        mDbQueryDoneConnection = mDbQuery->done.connect([this] {
            handleDbQueryResult();
        });
    }
}

void SessionPageModel::handleDbQueryResult()
{
    if (mDbQuery == nullptr) {
        SPDLOG_ERROR("Database query result handler called without request");
        return;
    }
    auto session = mDbQuery->getResultValue();
    if (mDbQuery->getResult() == System::Result::Ok and session.has_value()) {
        mSessionAnalyzeWorkflow.setSession(session.value());
        SPDLOG_DEBUG("Analyze session {} on {} {}",
                     session->getTrack().getTrackName(),
                     session->getSessionDate().asString(),
                     session->getSessionTime().asString());
    }
    mDbQuery.reset();
    mDbQueryDoneConnection->disconnect();
}

} // namespace Rapid::Android
