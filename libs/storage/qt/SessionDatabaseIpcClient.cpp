// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionDatabaseIpcClient.hpp"
#include "SessionDatabaseIpcInterface.h"
#include <JsonDeserializer.hpp>
#include <JsonSerializer.hpp>
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
    connect(call.get(), &QDBusPendingCallWatcher::finished, this, [this, result](QDBusPendingCallWatcher* self) {
        QDBusPendingReply<QString> call = *self;
        if (not call.isError()) {
            auto sessionPath = QString{call.argumentAt<0>()};
            auto sessionFile = QFile(sessionPath);
            auto resultStatus = Rapid::System::Result::Error;
            if (sessionFile.exists() && sessionFile.open(QFile::ReadOnly)) {
                auto content = sessionFile.readAll().toStdString();
                auto session = JsonDeserializer::Session::deserialize(content);
                if (session.has_value()) {
                    result->setResultValue(session.value());
                    resultStatus = System::Result::Ok;
                }
            } else {
                SPDLOG_ERROR("Failed to open/read session file {}", sessionPath.toStdString());
            }
            result->setResult(resultStatus);
            mPendingCalls.erase(self);
        }
    });
    mPendingCalls.insert({call.get(), call});
    return result;
}

std::shared_ptr<GetSessionMetaDataResult> SessionDatabaseIpcClient::getSessionMetaDataByIndexAsync(
    std::size_t index) noexcept
{
    auto call = std::make_shared<QDBusPendingCallWatcher>(mInterface->GetSessionMetaDataByIndex(index));
    auto result = std::make_shared<GetSessionMetaDataResult>();
    connect(call.get(), &QDBusPendingCallWatcher::finished, this, [this, result](QDBusPendingCallWatcher* self) {
        QDBusPendingReply<QString> call = *self;
        if (not call.isError()) {
            auto sessionPath = QString{call.argumentAt<0>()};
            auto sessionFile = QFile(sessionPath);
            auto resultStatus = Rapid::System::Result::Error;
            if (sessionFile.exists() && sessionFile.open(QFile::ReadOnly)) {
                auto content = sessionFile.readAll().toStdString();
                auto sessionMetaData = JsonDeserializer::SessionMetaData::deserialize(content);
                if (sessionMetaData.has_value()) {
                    result->setResultValue(sessionMetaData.value());
                    resultStatus = System::Result::Ok;
                }
            } else {
                SPDLOG_ERROR("Failed to open/read session file {}", sessionPath.toStdString());
            }
            result->setResult(resultStatus);
            mPendingCalls.erase(self);
        }
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

} // namespace Rapid::Storage::Qt
