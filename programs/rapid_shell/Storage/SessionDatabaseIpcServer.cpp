// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionDatabaseIpcServer.hpp"
#include <ISessionDatabase.hpp>
#include <JsonDeserializer.hpp>
#include <JsonSerializer.hpp>
#include <QDir>
#include <QStandardPaths>
#include <spdlog/spdlog.h>

namespace Rapid::RapidShell::Storage
{

struct SessionDatabaseIpcServerPrivate
{
    SessionDatabaseIpcServerPrivate(Rapid::Storage::ISessionDatabase& database, SessionDatabaseIpcServer* p)
        : mDatabase{database}
        , q{p}
    {
        mTempFolder = QDir::tempPath().append(QDir::separator()).append("rapid_shell");
        auto dir = QDir{mTempFolder};
        if (not dir.exists() and not dir.mkpath(mTempFolder)) {
            SPDLOG_CRITICAL("Failed create temp folder for data exchange.");
        }

        mDatabase.sessionAdded.connect([this](std::size_t index) {
            Q_EMIT q->SessionAdded(static_cast<quint32>(index));
        });

        mDatabase.sessionDeleted.connect([this](std::size_t index) {
            Q_EMIT q->SessionDeleted(static_cast<quint32>(index));
        });

        mDatabase.sessionUpdated.connect([this](std::size_t index) {
            Q_EMIT q->SessionUpdated(static_cast<quint32>(index));
        });
    }

    QDBusConnection mConnection = QDBusConnection::sessionBus();
    std::unordered_map<System::AsyncResult*, std::shared_ptr<Rapid::Storage::GetSessionResult>> mGetSessionRequests;
    std::unordered_map<System::AsyncResult*, std::shared_ptr<System::AsyncResult>> mStoreSessionRequests;
    Rapid::Storage::ISessionDatabase& mDatabase;
    QString mTempFolder;
    SessionDatabaseIpcServer* q;
};

SessionDatabaseIpcServer::SessionDatabaseIpcServer(Rapid::Storage::ISessionDatabase& database)
    : mD{std::make_unique<SessionDatabaseIpcServerPrivate>(database, this)}
{
    if (not mD->mConnection.registerService("de.rapid.shell")) {
        spdlog::error("Failed register service session database IPC server. Error: {}",
                      mD->mConnection.lastError().message().toStdString());
    }
    if (not mD->mConnection.registerObject("/de/rapid/shell",
                                           this,
                                           QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        spdlog::error("Failed register object session database IPC server. Error: {}",
                      mD->mConnection.lastError().message().toStdString());
    }
}

SessionDatabaseIpcServer::~SessionDatabaseIpcServer() = default;

quint32 SessionDatabaseIpcServer::GetSessionCount() noexcept
{
    return mD->mDatabase.getSessionCount();
}

QString SessionDatabaseIpcServer::GetSessionByIndex(quint32 index, QDBusMessage const& message) noexcept
{
    message.setDelayedReply(true);
    auto result = mD->mDatabase.getSessionByIndexAsync(index);
    mD->mGetSessionRequests.insert({result.get(), result});
    if (result->getResult() != System::Result::NotFinished) {
        handleGetSessionByIndex(result.get(), message);
    } else {
        result->done.connect([this, message](System::AsyncResult* result) {
            handleGetSessionByIndex(result, message);
        });
    }
    return {};
}

void SessionDatabaseIpcServer::handleGetSessionByIndex(System::AsyncResult* result, QDBusMessage const& message)
{
    auto reply = QDBusMessage{};
    if (result->getResult() == System::Result::Ok) {
        auto const valueResult = mD->mGetSessionRequests.at(result);
        auto const session = valueResult->getResultValue();
        if (session.has_value()) {
            auto const filePath = mD->mTempFolder.append(QDir::separator())
                                      .append("%1_%2.session")
                                      .arg(QString::fromStdString(session->getSessionDate().asString()),
                                           QString::fromStdString(session->getSessionTime().asString()));
            auto file = QFile{filePath};
            if (file.open(QFile::WriteOnly)) {
                auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(session.value());
                file.write(rawJson.c_str(), static_cast<qint64>(rawJson.size()));
                reply = message.createReply();
                reply << filePath;
            } else {
                reply = message.createErrorReply(QDBusError::Failed, "Failed to write session informations");
            }
        }
    } else {
        reply = message.createErrorReply(QDBusError::InvalidArgs, "No session found for passed index.");
    }
    mD->mGetSessionRequests.erase(result);
    mD->mConnection.send(reply);
}

bool SessionDatabaseIpcServer::StoreSession(QString const& sessionPath, QDBusMessage const& message) noexcept
{
    message.setDelayedReply(true);
    auto file = QFile{sessionPath};
    if (not file.exists(sessionPath)) {
        auto const reply =
            message.createErrorReply(QDBusError::Failed,
                                     QString{"No file exists for the passed path. Path: %1"}.arg(sessionPath));
        mD->mConnection.send(reply);
        return false;
    }
    if (not file.open(QFile::ReadOnly)) {
        auto const reply =
            message.createErrorReply(QDBusError::Failed, QString{"Failed to open file. Path: %1"}.arg(sessionPath));
        mD->mConnection.send(reply);
        return false;
    }
    auto const content = QByteArray{file.readAll()}.toStdString();
    auto const session = Common::JsonDeserializer::Session::deserialize(content);
    if (not session.has_value()) {
        auto const reply =
            message.createErrorReply(QDBusError::Failed,
                                     QString{"Failed deserialize the session data for: %1"}.arg(sessionPath));
        mD->mConnection.send(reply);
        return false;
    }
    auto result = mD->mDatabase.storeSession(session.value());
    mD->mStoreSessionRequests.insert({result.get(), result});
    if (result->getResult() != System::Result::NotFinished) {
        handleSessionStore(result.get(), message);
    } else {
        result->done.connect([this, message](System::AsyncResult* result) {
            handleSessionStore(result, message);
        });
    }
    return false;
}

void SessionDatabaseIpcServer::DeleteSessionByIndex(quint32 index)
{
    mD->mDatabase.deleteSession(index);
}

void SessionDatabaseIpcServer::handleSessionStore(System::AsyncResult* result, QDBusMessage const& message)
{
    bool success = result->getResult() == System::Result::Ok;
    auto reply = QDBusMessage{};
    if (success) {
        reply = message.createReply();
        reply << success;
    } else {
        reply = message.createErrorReply(QDBusError::Failed, "Failed to store session. Internal database error.");
    }
    mD->mConnection.send(reply);
}

} // namespace Rapid::RapidShell::Storage
