// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RESTGPSSOURCE_HPP
#define RESTGPSSOURCE_HPP

#include <QQmlApplicationEngine>
#include <QtGlobal>
#include <memory>

struct RestGpsSourcePrivate;
class RestGpsSource final
{
    Q_DISABLE_COPY_MOVE(RestGpsSource)
public:
    RestGpsSource();
    ~RestGpsSource();
    void show();

private:
    std::unique_ptr<RestGpsSourcePrivate> d;
};

#endif // RESTGPSSOURCE_HPP
