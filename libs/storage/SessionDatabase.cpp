// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionDatabase.hpp"
#include "JsonDeserializer.hpp"
#include "JsonSerializer.hpp"

using namespace Rapid::System;

namespace Rapid::Storage
{

SessionDatabase::SessionDatabase(ISessionDatabaseBackend& backend)
    : mBackend{backend}
{
}

SessionDatabase::~SessionDatabase() = default;

std::size_t SessionDatabase::getSessionCount()
{
    return mBackend.getNumberOfStoredSessions();
}

std::optional<Rapid::Common::SessionData> SessionDatabase::getSessionByIndex(std::size_t index) const noexcept
{
    auto const rawSession = mBackend.loadSessionByIndex(index);
    if (rawSession.empty()) {
        return std::nullopt;
    }

    auto sessionData = Common ::JsonDeserializer::deserializeSessionData(rawSession);
    if (!sessionData) {
        return std::nullopt;
    }

    return sessionData;
}

std::shared_ptr<System::AsyncResult> SessionDatabase::storeSession(Common::SessionData const& session)
{
    ArduinoJson::StaticJsonDocument<8192> jsonDoc;
    auto result = std::make_shared<AsyncResult>();
    auto jsonRootObject = jsonDoc.to<ArduinoJson::JsonObject>();
    if (!Common::JsonSerializer::serializeSessionData(session, jsonRootObject)) {
        result->setResult(System::Result::Error);
        return result;
    }

    // This is the case when the active session is stored.
    if (!mBackend.getIndexList().empty()) {
        for (std::size_t index = 0; index < mBackend.getNumberOfStoredSessions(); ++index) {
            auto const storedSession =
                Common::JsonDeserializer::deserializeSessionData(mBackend.loadSessionByIndex(index));

            if (storedSession && (storedSession->getSessionDate() == session.getSessionDate()) &&
                (storedSession->getSessionTime() == session.getSessionTime()) &&
                (storedSession->getTrack() == session.getTrack())) {
                auto const updated = mBackend.storeSession(index, jsonDoc.as<std::string>());
                if (updated) {
                    sessionUpdated.emit(index);
                }

                auto const updateResult = updated ? System::Result::Ok : System::Result::Error;
                result->setResult(updateResult);
                return result;
            }
        }
    }

    // This is the case when a new session is started.
    std::size_t storageIndex = (mBackend.getNumberOfStoredSessions() == 0) ? 0 : mBackend.getLastStoredIndex() + 1;
    auto const stored = mBackend.storeSession(storageIndex, jsonDoc.as<std::string>());
    if (stored) {
        sessionAdded.emit(storageIndex);
    }
    auto const storeResult = stored ? System::Result::Ok : System::Result::Error;
    result->setResult(storeResult);
    return result;
}

void SessionDatabase::deleteSession(std::size_t index)
{
    auto const deleted = mBackend.deleteSession(index);
    if (deleted) {
        sessionDeleted.emit(index);
    }
}

} // namespace Rapid::Storage
