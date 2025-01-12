// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionDatabaseIpcServer.hpp"
#include <QDir>
#include <QStandardPaths>
#include <common/JsonDeserializer.hpp>
#include <common/JsonSerializer.hpp>
#include <spdlog/spdlog.h>
#include <storage/ISessionDatabase.hpp>
#include <system/EventLoop.hpp>

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

        auto eval = System::EventLoop::instance().getConnectionEvaluator();
        std::ignore = mDatabase.sessionAdded.connectDeferred(eval, [this](std::size_t index) {
            Q_EMIT q->SessionAdded(static_cast<quint32>(index));
        });

        std::ignore = mDatabase.sessionDeleted.connectDeferred(eval, [this](std::size_t index) {
            Q_EMIT q->SessionDeleted(static_cast<quint32>(index));
        });

        std::ignore = mDatabase.sessionUpdated.connectDeferred(eval, [this](std::size_t index) {
            Q_EMIT q->SessionUpdated(static_cast<quint32>(index));
        });
    }

    QString getTempFolder() const noexcept
    {
        return mTempFolder;
    }

    QDBusConnection mConnection = QDBusConnection::sessionBus();
    std::unordered_map<System::AsyncResult*, std::shared_ptr<Rapid::Storage::GetSessionResult>> mGetSessionRequests;
    std::unordered_map<System::AsyncResult*, std::shared_ptr<Rapid::Storage::GetSessionMetaDataResult>>
        mGetSessionMetaDataRequests;
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
        std::ignore = result->done.connect([this, message](System::AsyncResult* result) {
            handleGetSessionByIndex(result, message);
        });
    }
    return {};
}

QString SessionDatabaseIpcServer::GetSessionByMetaData(QString const& sessionMetaPath, QDBusMessage const& message)
{
    message.setDelayedReply(true);
    auto maybeSessionMetadata = readSessionMetaDataJsonFile(sessionMetaPath);
    if (maybeSessionMetadata.has_value()) {
        auto result = mD->mDatabase.getSessionByMetadataAsync(maybeSessionMetadata.value());
        mD->mGetSessionRequests.insert({result.get(), result});
        if (result->getResult() != System::Result::NotFinished) {
            handleGetSessionByMetadata(result.get(), message);
        } else {
            std::ignore = result->done.connect([this, message](System::AsyncResult* result) {
                handleGetSessionByMetadata(result, message);
            });
        }
    } else {
        auto reply = message.createErrorReply(QDBusError::InvalidArgs, "No session found for passed meta data.");
        mD->mConnection.send(reply);
    }
    return {};
}

QString SessionDatabaseIpcServer::GetSessionMetaDataByIndex(quint32 index, QDBusMessage const& message) noexcept
{
    message.setDelayedReply(true);
    auto result = mD->mDatabase.getSessionMetaDataByIndexAsync(index);
    mD->mGetSessionMetaDataRequests.insert({result.get(), result});
    if (result->getResult() != System::Result::NotFinished) {
        handleGetSessionMetaDataByIndex(result.get(), message);
    } else {
        std::ignore = result->done.connect([this, message](System::AsyncResult* result) {
            handleGetSessionMetaDataByIndex(result, message);
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
            auto const filePath = mD->getTempFolder()
                                      .append(QDir::separator())
                                      .append("%1_%2.session")
                                      .arg(QString::fromStdString(session->getSessionDate().asString()),
                                           QString::fromStdString(session->getSessionTime().asString()));
            auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(session.value());
            if (writeJsonFile(filePath, rawJson)) {
                reply = message.createReply();
                reply << filePath;
            } else {
                reply = message.createErrorReply(QDBusError::Failed, "Failed to write session informations");
            }
        }
    } else {
        reply = message.createErrorReply(QDBusError::InvalidArgs, "No session found for passed index.");
    }
    mD->mConnection.send(reply);
    mD->mGetSessionRequests.erase(result);
}

void SessionDatabaseIpcServer::handleGetSessionByMetadata(System::AsyncResult* result, QDBusMessage const& msg)
{
    auto reply = QDBusMessage{};
    if (result->getResult() == System::Result::Ok) {
        auto const valueResult = mD->mGetSessionRequests.at(result);
        auto const maybeSession = valueResult->getResultValue();
        if (maybeSession.has_value()) {
            auto maybeFilePath = writeSession(maybeSession.value());
            if (maybeFilePath.has_value()) {
                reply = msg.createReply();
                reply << maybeFilePath.value();
            }
        } else {
            reply = msg.createErrorReply(QDBusError::Failed, "Failed to write session informations");
        }
    } else {
        reply = msg.createErrorReply(QDBusError::Failed, "No Session found");
    }
    mD->mGetSessionRequests.erase(result);
    mD->mConnection.send(reply);
}

void SessionDatabaseIpcServer::handleGetSessionMetaDataByIndex(System::AsyncResult* result, QDBusMessage const& msg)
{
    auto reply = QDBusMessage{};
    if (result->getResult() == System::Result::Ok) {
        auto const valueResult = mD->mGetSessionMetaDataRequests.at(result);
        auto const sessionMetaData = valueResult->getResultValue();
        if (sessionMetaData.has_value()) {
            auto const filePath = mD->getTempFolder()
                                      .append(QDir::separator())
                                      .append("%1_%2.sessionMetaData")
                                      .arg(QString::fromStdString(sessionMetaData->getSessionDate().asString()),
                                           QString::fromStdString(sessionMetaData->getSessionTime().asString()));
            auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(sessionMetaData.value());
            if (writeJsonFile(filePath, rawJson)) {
                reply = msg.createReply();
                reply << filePath;
            } else {
                reply = msg.createErrorReply(QDBusError::Failed, "Failed to write session meta data informations");
            }
        }
    } else {
        reply = msg.createErrorReply(QDBusError::InvalidArgs, "No session found for passed index.");
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
        std::ignore = result->done.connect([this, message](System::AsyncResult* result) {
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

std::optional<QString> SessionDatabaseIpcServer::writeSession(Common::SessionData const& session) const noexcept
{
    auto const filePath = mD->getTempFolder()
                              .append(QDir::separator())
                              .append("%1_%2.session")
                              .arg(QString::fromStdString(session.getSessionDate().asString()),
                                   QString::fromStdString(session.getSessionTime().asString()));
    auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(session);
    if (not writeJsonFile(filePath, rawJson)) {
        return std::nullopt;
    }
    return filePath;
}

bool SessionDatabaseIpcServer::writeJsonFile(QString const& path, std::string const& rawJson) const noexcept
{
    auto file = QFile{path};
    if (not file.open(QFile::WriteOnly)) {
        SPDLOG_CRITICAL("Failed to open file {}", file.fileName().toStdString());
        return false;
    }
    auto size = file.write(rawJson.c_str(), static_cast<qint64>(rawJson.size()));
    if (size < static_cast<qint64>(rawJson.size())) {
        SPDLOG_CRITICAL("Failed to write file conent for {}", file.fileName().toStdString());
        return false;
    }
    return true;
}

std::optional<Common::SessionMetaData> SessionDatabaseIpcServer::readSessionMetaDataJsonFile(QString const& path)
{
    auto file = QFile{path};
    if (not file.exists()) {
        SPDLOG_ERROR("Failed to read session meta data file {}. File not found", path.toStdString());
        return std::nullopt;
    }
    if (not file.open(QFile::ReadOnly)) {
        SPDLOG_ERROR("Failed to open session meta data file {}.", path.toStdString());
        return std::nullopt;
    }
    auto content = file.readAll().toStdString();
    auto maybeSessionMetadata = Common::JsonDeserializer::SessionMetaData::deserialize(content);
    if (not maybeSessionMetadata.has_value()) {
        SPDLOG_ERROR("Failed to deserialize session meta data file {}.", path.toStdString());
        return std::nullopt;
    }
    return maybeSessionMetadata;
}

} // namespace Rapid::RapidShell::Storage
