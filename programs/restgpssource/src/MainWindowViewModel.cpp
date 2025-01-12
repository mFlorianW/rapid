// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MainWindowViewModel.hpp"
#include "PositionListModel.hpp"
#include "RestHttpClient.hpp"
#include <QDebug>
#include <QUrl>
#include <positioning/ConstantGpsPositionProvider.hpp>
#include <positioning/qt/CsvGpsFileReader.hpp>

struct MainWindowViewModelPrivate
{
    PositionListModel mPositionListModel;
    Rapid::Positioning::ConstantGpsPositionProvider mGpsProvider;
    QGeoCoordinate mCurrentPosition;
    bool mGpsSourceActive{false};
    QString mHostAddress{"localhost"};
    QString mHostPort{"3000"};
    RestHttpClient mRestHttpClient;
};

MainWindowViewModel::MainWindowViewModel()
    : d{std::make_unique<MainWindowViewModelPrivate>()}
{
    std::ignore = d->mGpsProvider.gpsPosition.valueChanged().connect(&MainWindowViewModel::handlePositionUpdate, this);
    d->mGpsProvider.setVelocityInMeterPerSecond(80.6667);

    updateUrl();
}

MainWindowViewModel::~MainWindowViewModel() = default;

QAbstractItemModel* MainWindowViewModel::getPositionModel() const noexcept
{
    return &d->mPositionListModel;
}

void MainWindowViewModel::startGpsSource()
{
    d->mGpsProvider.start();
    d->mGpsSourceActive = true;
    Q_EMIT gpsSourceActiveChanged();
}

void MainWindowViewModel::stopGpsSource()
{
    d->mGpsProvider.stop();
    d->mPositionListModel.clear();
    d->mGpsSourceActive = false;
    Q_EMIT gpsSourceActiveChanged();
}

void MainWindowViewModel::loadGpsFile(QUrl const& fileName)
{
    auto csvReader = Rapid::Positioning::Qt::CsvGpsFileReader{};
    csvReader.setFileName(fileName.toLocalFile());
    if (!csvReader.read()) {
        qCritical() << "Failed to read GpsFile:" << fileName;
    }

    d->mGpsProvider.setGpsPositions(csvReader.getPostions());
}

void MainWindowViewModel::handlePositionUpdate()
{
    auto const position = d->mGpsProvider.gpsPosition.get();
    d->mPositionListModel.addPosition(position);
    d->mCurrentPosition = QGeoCoordinate{position.getPosition().getLatitude(), position.getPosition().getLongitude()};
    d->mRestHttpClient.sendPosition(position);
    Q_EMIT currentPositionChanged();
}

QGeoCoordinate MainWindowViewModel::getCurrentPosition() const noexcept
{
    return d->mCurrentPosition;
}

bool MainWindowViewModel::isGpsSourceActive() const noexcept
{
    return d->mGpsSourceActive;
}

QString MainWindowViewModel::getHostAddress() const noexcept
{
    return d->mHostAddress;
}

void MainWindowViewModel::setHostAddress(QString const& hostAddress)
{
    if (d->mHostAddress != hostAddress) {
        d->mHostAddress = hostAddress;
        updateUrl();
        Q_EMIT hostAddressChanged();
    }
}

QString MainWindowViewModel::getHostPort() const noexcept
{
    return d->mHostPort;
}

void MainWindowViewModel::setHostPort(QString const& hostPort)
{
    if (d->mHostPort != hostPort) {
        d->mHostPort = hostPort;
        updateUrl();
        Q_EMIT hostPortChanged();
    }
}

void MainWindowViewModel::updateUrl()
{
    auto const newUrl = QString{"http://%1:%2"}.arg(d->mHostAddress, d->mHostPort);
    d->mRestHttpClient.setUrl(newUrl);
}
