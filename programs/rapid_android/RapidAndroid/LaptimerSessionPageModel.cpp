// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LaptimerSessionPageModel.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::Android
{

LaptimerSessionPageModel::LaptimerSessionPageModel(std::unique_ptr<Storage::ISessionDatabase> sessionDb)
    : mRestSessionManagement{std::make_unique<Workflow::Qt::RestSessionManagementWorkflow>()}
    , mSessionDatabase{std::move(sessionDb)}
{
    mDownloadFinishedConnection =
        mRestSessionManagement->sessionDownloadFinshed.connect([this](auto&& index, auto&& result) {
            if (result == Workflow::DownloadResult::Ok) {
                SPDLOG_INFO("Session download finished for index {} {}", index, static_cast<int>(result));
                auto session = mRestSessionManagement->getSession(index);
                if (session.has_value()) {
                    mStoreResult = mSessionDatabase->storeSession(session.value());
                    mSessionStoreConnection = mStoreResult->done.connect([this] {
                        mIsDownloading = false;
                        Q_EMIT downloadingChanged();
                    });
                }
            }
        });

    mSessionSortModel.setSourceModel(mRestSessionManagement->getSessionMetadataListModel().get());
}

LaptimerSessionPageModel::~LaptimerSessionPageModel() = default;

void LaptimerSessionPageModel::updateSessionMetadata() noexcept
{
    mRestSessionManagement->fetchSessionCount();
    mRestSessionManagement->downloadAllSessionMetadata();
}

void LaptimerSessionPageModel::downloadSession(std::size_t index) noexcept
{
    if (index > mRestSessionManagement->getSessionCount()) {
        SPDLOG_ERROR("Failed to start session download for index {}. Error: index out of range. Maximum range is {}",
                     index,
                     mRestSessionManagement->getSessionCount());
        return;
    }
    auto const downloadIndex = mSessionSortModel.mapToSource(mSessionSortModel.index(static_cast<int>(index), 0));
    SPDLOG_DEBUG("Start download for index {}", downloadIndex.row());
    mIsDownloading = true;
    mRestSessionManagement->downloadSession(downloadIndex.row());
    Q_EMIT downloadingChanged();
}

Rapid::Common::Qt::DeviceSettings LaptimerSessionPageModel::getActiveLaptimer() const noexcept
{
    return mActiveLaptimer;
}

void LaptimerSessionPageModel::setActiveLaptimer(Rapid::Common::Qt::DeviceSettings activeLaptimer) noexcept
{
    if (mActiveLaptimer != activeLaptimer) {
        mActiveLaptimer = activeLaptimer;
        mRestclient.setServerAddress(mActiveLaptimer.getIpAddress().toStdString());
        mRestclient.setServerPort(mActiveLaptimer.port);
        mRestSessionManagement->setRestClient(&mRestclient);
        Q_EMIT activeLaptimerChanged();
    }
}

Rapid::Common::Qt::SessionMetaDataSortListModel* LaptimerSessionPageModel::getSessionMetadataListModel() noexcept
{
    return &mSessionSortModel;
}

bool LaptimerSessionPageModel::isDownloading() const noexcept
{
    return mIsDownloading;
}

} // namespace Rapid::Android
