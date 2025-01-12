// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionDatabaseIpcClient.hpp"
#include "SessionDatabaseIpcInterface.h"
#include <common/JsonDeserializer.hpp>
#include <common/JsonSerializer.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::Common;

namespace Rapid::Storage::Qt
{

SessionDatabaseIpcClient::SessionDatabaseIpcClient()
    : mInterface{std::make_unique<DeRapidShellSessionDatabaseInterface>(QString{"de.rapid.shell"},
                                                                        QString{"/de/rapid/shell"},
                                                                        QDBusConnection::sessionBus())}
{
    auto call = std::make_shared<QDBusPendingCallWatcher>(mInterface->GetSessionCount());
    mPendingCalls.insert({call.get(), call});
    QObject::connect(call.get(), &QDBusPendingCallWatcher::finished, call.get(), [this](QDBusPendingCallWatcher* self) {
        QDBusPendingReply<uint> call = *self;
        if (not call.isError()) {
            mSessionCount = call.argumentAt<0>();
            mInitialized = true;
            Q_EMIT initialized();
            mPendingCalls.erase(self);
        }
    });

    connect(mInterface.get(), &DeRapidShellSessionDatabaseInterface::SessionDeleted, this, [this](std::size_t index) {
        sessionDeleted.emit(index);
    });
    connect(mInterface.get(), &DeRapidShellSessionDatabaseInterface::SessionAdded, this, [this](std::size_t index) {
        sessionAdded.emit(index);
    });
    connect(mInterface.get(), &DeRapidShellSessionDatabaseInterface::SessionUpdated, this, [this](std::size_t index) {
        sessionUpdated.emit(index);
    });
}

SessionDatabaseIpcClient::~SessionDatabaseIpcClient() = default;

bool SessionDatabaseIpcClient::isInitialized()
{
    return mInitialized;
}

std::size_t SessionDatabaseIpcClient::getSessionCount()
{
    return mSessionCount;
}

std::optional<Common::SessionData> SessionDatabaseIpcClient::getSessionByIndex(std::size_t index) const noexcept
{
    return std::nullopt;
}

std::shared_ptr<GetSessionResult> SessionDatabaseIpcClient::getSessionByIndexAsync(std::size_t index) noexcept
{
    auto call = std::make_shared<QDBusPendingCallWatcher>(mInterface->GetSessionByIndex(index));
    auto result = std::make_shared<GetSessionResult>();
    mPendingCalls.insert({call.get(), call});
    connect(call.get(), &QDBusPendingCallWatcher::finished, this, [this, result](auto* self) {
        handleSessionResponse(self, result);
    });
    return result;
}

std::shared_ptr<GetSessionResult> SessionDatabaseIpcClient::getSessionByMetadataAsync(
    Common::SessionMetaData const& session) noexcept
{
    auto result = std::make_shared<GetSessionResult>();
    auto filePath = QString{"%1_%2.sessionMetaData"}.arg(QString::fromStdString(session.getSessionDate().asString()),
                                                         QString::fromStdString(session.getSessionTime().asString()));
    auto content = JsonSerializer::Session::serialize(session);
    auto mayebeFilePath = writeExchangeFile(filePath, content);
    if (not mayebeFilePath.has_value()) {
        SPDLOG_ERROR("Failed to write exchange with server");
        result->setResult(System::Result::Error, std::string{"Failed write exchange file"});
        return result;
    }
    auto call = std::make_shared<QDBusPendingCallWatcher>(mInterface->GetSessionByMetaData(mayebeFilePath.value()));
    mPendingCalls.insert({call.get(), call});
    connect(call.get(), &QDBusPendingCallWatcher::finished, this, [this, result](auto* self) {
        handleSessionResponse(self, result);
    });
    return result;
}

std::shared_ptr<GetSessionMetaDataResult> SessionDatabaseIpcClient::getSessionMetaDataByIndexAsync(
    std::size_t index) noexcept
{
    auto call = std::make_shared<QDBusPendingCallWatcher>(mInterface->GetSessionMetaDataByIndex(index));
    auto result = std::make_shared<GetSessionMetaDataResult>();
    connect(call.get(), &QDBusPendingCallWatcher::finished, this, [this, result](auto* self) {
        handleSessionMetaDataResponse(self, result);
    });
    mPendingCalls.insert({call.get(), call});
    return result;
}

std::shared_ptr<System::AsyncResult> SessionDatabaseIpcClient::storeSession(Common::SessionData const& session)
{
    auto result = std::make_shared<System::AsyncResult>();
    auto dirPath = QDir{QDir::tempPath().append(QDir::separator()).append("rapid_shell")};
    auto filePath = dirPath.path()
                        .append(QDir::separator())
                        .append("%1_%2.session")
                        .arg(QString::fromStdString(session.getSessionDate().asString()),
                             QString::fromStdString(session.getSessionTime().asString()));
    if (not dirPath.exists()) {
        dirPath.mkpath(dirPath.path());
    }
    auto sessionFile = QFile{filePath};
    if (not sessionFile.open(QFile::WriteOnly)) {
        SPDLOG_ERROR("Failed to write session file {} for exchange with server", filePath.toStdString());
        result->setResult(System::Result::Error);
        return result;
    }
    auto content = JsonSerializer::Session::serialize(session);
    sessionFile.write(content.c_str(), static_cast<qint64>(content.size()));
    auto call = std::make_shared<QDBusPendingCallWatcher>(mInterface->StoreSession(filePath));
    connect(call.get(), &QDBusPendingCallWatcher::finished, this, [this, result](QDBusPendingCallWatcher* self) {
        auto resultStatus = System::Result::Error;
        auto call = QDBusPendingReply<bool>{*self};
        if (not call.isError() && call.argumentAt<0>()) {
            resultStatus = System::Result::Ok;
        } else {
            SPDLOG_ERROR("Store operation failed. Error: {}", call.error().message().toStdString());
        }
        result->setResult(resultStatus);
        mPendingCalls.erase(self);
    });
    mPendingCalls.insert({call.get(), call});
    return result;
}

void SessionDatabaseIpcClient::deleteSession(std::size_t index)
{
    auto call = std::make_shared<QDBusPendingCallWatcher>(mInterface->DeleteSessionByIndex(index));
    QObject::connect(call.get(), &QDBusPendingCallWatcher::finished, call.get(), [this](QDBusPendingCallWatcher* self) {
        mPendingCalls.erase(self);
    });
    mPendingCalls.insert({call.get(), call});
}

void SessionDatabaseIpcClient::handleSessionResponse(QDBusPendingCallWatcher* self,
                                                     std::shared_ptr<GetSessionResult> result)
{
    QDBusPendingReply<QString> call = *self;
    auto resultStatus = Rapid::System::Result::Error;
    if (not call.isError()) {
        auto sessionPath = QString{call.argumentAt<0>()};
        auto maybeSession = readExchangedSession(sessionPath);
        if (maybeSession.has_value()) {
            resultStatus = System::Result::Ok;
            result->setResultValue(maybeSession.value());
        } else {
            SPDLOG_ERROR("Failed to open/read session file {}", sessionPath.toStdString());
        }
    }
    mPendingCalls.erase(self);
    result->setResult(resultStatus);
}

void SessionDatabaseIpcClient::handleSessionMetaDataResponse(
    QDBusPendingCallWatcher* self,
    std::shared_ptr<Rapid::Storage::GetSessionMetaDataResult> result)
{
    QDBusPendingReply<QString> call = *self;
    auto resultStatus = Rapid::System::Result::Error;
    if (not call.isError()) {
        auto sessionPath = QString{call.argumentAt<0>()};
        auto maybeSessionMeta = readExchangedSessionMetaData(sessionPath);
        if (maybeSessionMeta.has_value()) {
            resultStatus = System::Result::Ok;
            result->setResultValue(maybeSessionMeta.value());
        } else {
            SPDLOG_ERROR("Failed to open/read session file {}", sessionPath.toStdString());
        }
    } else {
        SPDLOG_ERROR("SessionMetaData request DBus call finished with an error: {}",
                     call.error().message().toStdString());
    }
    result->setResult(resultStatus);
    mPendingCalls.erase(self);
}

std::optional<QString> SessionDatabaseIpcClient::writeExchangeFile(QString const& fileName,
                                                                   std::string const& content) const noexcept
{
    auto dirPath = QDir{QDir::tempPath().append(QDir::separator()).append("rapid_shell")};
    auto filePath = dirPath.path().append(QDir::separator()).append(fileName);
    if (not dirPath.exists() and not dirPath.mkpath(dirPath.path())) {
        SPDLOG_ERROR("Failed to create temp folder for exchange with server", dirPath.path().toStdString());
    }
    auto sessionFile = QFile{filePath};
    if (not sessionFile.open(QFile::WriteOnly)) {
        SPDLOG_ERROR("Failed to open session file {} for exchange with server", filePath.toStdString());
        return std::nullopt;
    }
    auto writtenBytes = sessionFile.write(content.c_str(), static_cast<qint64>(content.size()));
    if (static_cast<std::size_t>(writtenBytes) < content.size()) {
        SPDLOG_ERROR("Failed to write session file {} for exchange with server", filePath.toStdString());
        return std::nullopt;
    }
    return filePath;
}

std::optional<Common::SessionData> SessionDatabaseIpcClient::readExchangedSession(QString const& path) const noexcept
{
    auto sessionFile = QFile(path);
    if (not sessionFile.exists()) {
        SPDLOG_ERROR("Exchange file {} doesn't exists.", path.toStdString());
        return std::nullopt;
    }
    if (not sessionFile.open(QFile::ReadOnly)) {
        SPDLOG_ERROR("Failed to open file {} .", path.toStdString());
        return std::nullopt;
    }
    auto content = sessionFile.readAll().toStdString();
    auto maybeSession = JsonDeserializer::Session::deserialize(content);
    if (not maybeSession.has_value()) {
        return std::nullopt;
    }
    return maybeSession;
}

std::optional<Common::SessionMetaData> SessionDatabaseIpcClient::readExchangedSessionMetaData(
    QString const& path) const noexcept
{
    auto sessionFile = QFile(path);
    if (not sessionFile.exists()) {
        SPDLOG_ERROR("Exchange file {} doesn't exists.", path.toStdString());
        return std::nullopt;
    }
    if (not sessionFile.open(QFile::ReadOnly)) {
        SPDLOG_ERROR("Failed to open file {} .", path.toStdString());
        return std::nullopt;
    }
    auto content = sessionFile.readAll().toStdString();
    auto maybeSession = JsonDeserializer::SessionMetaData::deserialize(content);
    if (not maybeSession.has_value()) {
        return std::nullopt;
    }
    return maybeSession;
}

} // namespace Rapid::Storage::Qt
