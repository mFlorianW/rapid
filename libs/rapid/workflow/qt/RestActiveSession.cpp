// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestActiveSession.hpp"

namespace Rapid::Workflow::Qt
{

RestActiveSession::~RestActiveSession() = default;

RestActiveSession::RestActiveSession(Rest::IRestClient* restClient)
    : Rapid::Workflow::RestActiveSession{restClient}
{
    mTrackChangedConnection = track.valueChanged().connect([this] {
        Q_EMIT trackNameChanged();
    });

    mLapCountConnection = lapCount.valueChanged().connect([this] {
        Q_EMIT lapCountChanged();
    });

    mCurrentLapTimeConnection = currentLap.valueChanged().connect([this] {
        Q_EMIT currentLapTimeChanged();
    });

    mCurrentSectorTimeConnection = currentSector.valueChanged().connect([this] {
        Q_EMIT currentSectorTimeChanged();
    });
}

QString RestActiveSession::getTrackName() const noexcept
{
    return QString::fromStdString(track.get().getTrackName());
}

quint64 RestActiveSession::getLapCount() const noexcept
{
    return lapCount.get();
}

QString RestActiveSession::getCurrentLapTime() const noexcept
{
    return QString::fromStdString(currentLap.get().asString());
}

QString RestActiveSession::getCurrentSectorTime() const noexcept
{
    return QString::fromStdString(currentSector.get().asString());
}

void RestActiveSession::updateTrackData() noexcept
{
    Workflow::RestActiveSession::updateTrackData();
}

Q_INVOKABLE void RestActiveSession::updateLapData() noexcept
{
    Workflow::RestActiveSession::updateLapData();
}

} // namespace Rapid::Workflow::Qt
