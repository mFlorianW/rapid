// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QObject>

class SessionDatabaseAdaptor;

namespace Rapid
{

namespace Storage
{
class ISessionDatabase;
}

namespace Common
{
class SessionMetaData;
class SessionData;
} // namespace Common

namespace System
{
class AsyncResult;
}

} // namespace Rapid

namespace Rapid::RapidShell::Storage
{

struct SessionDatabaseIpcServerPrivate;
class SessionDatabaseIpcServer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "de.rapid.shell.SessionDatabase")
public:
    explicit SessionDatabaseIpcServer(Rapid::Storage::ISessionDatabase& database);
    ~SessionDatabaseIpcServer() override;

    SessionDatabaseIpcServer(SessionDatabaseIpcServer const&) = delete;
    SessionDatabaseIpcServer& operator=(SessionDatabaseIpcServer const&) = delete;
    SessionDatabaseIpcServer(SessionDatabaseIpcServer&&) noexcept = delete;
    SessionDatabaseIpcServer& operator=(SessionDatabaseIpcServer&&) noexcept = delete;

Q_SIGNALS:
    void SessionAdded(quint32 index);
    void SessionDeleted(quint32 index);
    void SessionUpdated(quint32 index);

public Q_SLOTS:
    quint32 GetSessionCount() noexcept;
    QString GetSessionByIndex(quint32 index, QDBusMessage const& message) noexcept;
    QString GetSessionByMetaData(QString const& sessionMetaPath, QDBusMessage const& message);
    QString GetSessionMetaDataByIndex(quint32 index, QDBusMessage const& message) noexcept;
    void DeleteSessionByIndex(quint32 index);
    bool StoreSession(QString const& sessionPath, QDBusMessage const& message) noexcept;

private:
    void handleGetSessionByIndex(System::AsyncResult* result, QDBusMessage const& msg);
    void handleGetSessionByMetadata(System::AsyncResult* result, QDBusMessage const& msg);
    void handleGetSessionMetaDataByIndex(System::AsyncResult* result, QDBusMessage const& msg);
    void handleSessionStore(System::AsyncResult* result, QDBusMessage const& message);
    std::optional<QString> writeSession(Common::SessionData const& session) const noexcept;
    bool writeJsonFile(QString const& path, std::string const& rawJson) const noexcept;
    std::optional<Common::SessionMetaData> readSessionMetaDataJsonFile(QString const& path);

private:
    std::unique_ptr<SessionDatabaseIpcServerPrivate> mD;
};

} // namespace Rapid::RapidShell::Storage
