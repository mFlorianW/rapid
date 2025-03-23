// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsPageModel.hpp"
#include <spdlog/spdlog.h>

using namespace Rapid::Positioning;

namespace Rapid::Android
{

GpsPageModel::GpsPageModel()
    : mGpsRestResourceReader{&mRestClient}
{
    using namespace std::chrono_literals;
    mUpdateTimer.setInterval(1s);
    connect(&mUpdateTimer, &QTimer::timeout, this, [this] {
        fetchFixModeResult();
        fetchSatellites();
    });
}

GpsPageModel::~GpsPageModel() = default;

void GpsPageModel::start()
{
    if (not mUpdateTimer.isActive()) {
        mUpdateTimer.start();
        Q_EMIT activeIconChanged();
    }
}

void GpsPageModel::stop()
{
    if (mUpdateTimer.isActive()) {
        mUpdateTimer.stop();
        Q_EMIT activeIconChanged();
    }
}

Rapid::Common::Qt::DeviceSettings GpsPageModel::getActiveLaptimer() const noexcept
{
    return mActiveLaptimer;
}

void GpsPageModel::setActiveLaptimer(Rapid::Common::Qt::DeviceSettings activeLaptimer) noexcept
{
    if (mActiveLaptimer != activeLaptimer) {
        mActiveLaptimer = activeLaptimer;
        mRestClient.setServerAddress(mActiveLaptimer.getIpAddress().toStdString());
        mRestClient.setServerPort(mActiveLaptimer.port);
        Q_EMIT activeLaptimerChanged();
    }
}

QString GpsPageModel::getGpsStatus() const noexcept
{
    switch (mFixMode) {
    case GpsFixMode::Fix3d:
        return QStringLiteral("qrc:/qt/qml/Rapid/Android/img/Wifi-High.svg");
    case GpsFixMode::Fix2d:
        return QStringLiteral("qrc:/qt/qml/Rapid/Android/img/Wifi-Low.svg");
    default:
        return QStringLiteral("qrc:/qt/qml/Rapid/Android/img/Wifi-None.svg");
    }
}

QString GpsPageModel::getSatellites() const noexcept
{
    return QString::number(mSatellites);
}

bool GpsPageModel::isRunning() const noexcept
{
    return mUpdateTimer.isActive();
}

QString GpsPageModel::getActiveIcon() const noexcept
{
    if (mUpdateTimer.isActive()) {
        return QStringLiteral("qrc:/qt/qml/Rapid/Android/img/Pause.svg");
    }
    return QStringLiteral("qrc:/qt/qml/Rapid/Android/img/Play.svg");
}

void GpsPageModel::fetchFixModeResult()
{
    mFixModeResult = mGpsRestResourceReader.getFixMode();
    mFixModeResultConnection = mFixModeResult->done.connect([this] {
        if (mFixModeResult->getResult() == System::Result::Ok) {
            mFixMode = mFixModeResult->getResultValue().value_or(Positioning::GpsFixMode::NoFix);
            Q_EMIT gpsStatusChanged();
        }
    });
}

void GpsPageModel::fetchSatellites()
{
    mSatellitesResult = mGpsRestResourceReader.getSatellites();
    mSatellitesResultConnection = mSatellitesResult->done.connect([this] {
        if (mSatellitesResult->getResult() == System::Result::Ok) {
            mSatellites = mSatellitesResult->getResultValue().value_or(0);
            Q_EMIT satellitesChanged();
        }
    });
}

} // namespace Rapid::Android
