// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestHttpClient.hpp"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <common/GpsPositionData.hpp>

struct RestHttpClientPrivate
{
    QUrl mServerUrl;
    QNetworkAccessManager mNetworkAccessManager;
};

RestHttpClient::RestHttpClient()
    : d{std::make_unique<RestHttpClientPrivate>()}
{
}

RestHttpClient::~RestHttpClient() = default;

void RestHttpClient::setUrl(QString const& url)
{
    d->mServerUrl = QString{url}.append("/position");
    qInfo() << "New Server URL:" << d->mServerUrl;
}

void RestHttpClient::sendPosition(Rapid::Common::GpsPositionData const& position)
{
    auto request = QNetworkRequest{d->mServerUrl};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "Content-Type: application/json");

    auto positionObject = QJsonObject{{"latitude", {position.getPosition().getLatitude()}},
                                      {"longitude", {position.getPosition().getLongitude()}},
                                      {"date", {QString::fromStdString(position.getDate().asString())}},
                                      {"time", {QString::fromStdString(position.getTime().asString())}}};
    auto body = QJsonDocument{positionObject};

    auto* reply = d->mNetworkAccessManager.post(request, body.toJson());
    connect(&d->mNetworkAccessManager, &QNetworkAccessManager::finished, reply, [](QNetworkReply* reply) {
        if (reply->isFinished() && reply->error() != QNetworkReply::NoError) {
            qCritical() << "Updating position failed with an error:" << reply->error();
        }
    });
}
