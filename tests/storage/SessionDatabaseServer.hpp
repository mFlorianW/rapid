// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDBusConnection>
#include <QObject>
#include <trompeloeil.hpp>

class SessionDatabaseServer : public QObject
{
    Q_OBJECT
public:
    SessionDatabaseServer()
        : mConnection{QDBusConnection::sessionBus()}
    {
        mConnection.registerObject("/de/rapid/shell", this);
        mConnection.registerService("de.rapid.shell");
    }
    ~SessionDatabaseServer() override = default;
    Q_DISABLE_COPY_MOVE(SessionDatabaseServer)

    MAKE_MOCK(GetSessionCount, auto()->uint);
    MAKE_MOCK(DeleteSessionByIndex, auto(uint)->void);
    MAKE_MOCK(GetSessionByIndex, auto(uint)->QString);
    MAKE_MOCK(GetSessionByMetaData, auto(QString)->QString);
    MAKE_MOCK(GetSessionMetaDataByIndex, auto(uint)->QString);
    MAKE_MOCK(StoreSession, auto(QString)->bool);

private:
    QDBusConnection mConnection;
};
