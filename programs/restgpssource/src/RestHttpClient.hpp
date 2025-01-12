// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QtGlobal>
#include <memory>

namespace Rapid::Common
{
class GpsPositionData;
}

struct RestHttpClientPrivate;
class RestHttpClient final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(RestHttpClient)
public:
    RestHttpClient();
    ~RestHttpClient() override;

    void setUrl(QString const& url);
    void sendPosition(Rapid::Common::GpsPositionData const& position);

private:
    std::unique_ptr<RestHttpClientPrivate> d;
};
