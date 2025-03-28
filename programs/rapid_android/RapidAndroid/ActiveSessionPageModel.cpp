// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ActiveSessionPageModel.hpp"

namespace Rapid::Android
{

ActiveSessionPageModel::ActiveSessionPageModel() = default;

ActiveSessionPageModel::~ActiveSessionPageModel() noexcept = default;

Rapid::Workflow::Qt::RestActiveSession* ActiveSessionPageModel::getActiveSession() noexcept
{
    return std::addressof(mRestActiveSession);
}

Rapid::Common::Qt::DeviceSettings ActiveSessionPageModel::getActiveLaptimer() const noexcept
{
    return mActiveLaptimer;
}

void ActiveSessionPageModel::setActiveLaptimer(Rapid::Common::Qt::DeviceSettings activeLaptimer)
{
    if (mActiveLaptimer != activeLaptimer) {
        mActiveLaptimer = activeLaptimer;
        mRestclient.setServerAddress(activeLaptimer.getIpAddress().toStdString());
        mRestclient.setServerPort(activeLaptimer.port);
        Q_EMIT activeLaptimerChanged();
    }
}

} // namespace Rapid::Android
