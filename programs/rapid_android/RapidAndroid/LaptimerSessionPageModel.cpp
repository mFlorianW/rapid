// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LaptimerSessionPageModel.hpp"

namespace Rapid::Android
{

LaptimerSessionPageModel::LaptimerSessionPageModel()
    : mRestSessionManagement{std::make_unique<Workflow::Qt::RestSessionManagementWorkflow>()}
{
}

LaptimerSessionPageModel::~LaptimerSessionPageModel() = default;

void LaptimerSessionPageModel::updateSessionMetadata() noexcept
{
    mRestSessionManagement->downloadAllSessionMetadata();
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

Rapid::Common::Qt::SessionMetaDataListModel* LaptimerSessionPageModel::getSessionMetadataListModel() noexcept
{
    return mRestSessionManagement->getSessionMetadataListModel().get();
}

} // namespace Rapid::Android
