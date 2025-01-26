// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionEndpoint.hpp"
#include <charconv>
#include <common/JsonSerializer.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace Rapid::Rest
{
constexpr auto endpointIdentifier = "sessions";

namespace
{
std::optional<std::size_t> getSessionIndex(std::string_view index) noexcept;
}

SessionEndpoint::SessionEndpoint(Storage::ISessionDatabase& database) noexcept
    : mDb{database}
{
}

void SessionEndpoint::handleRestRequest(RestRequest& request) noexcept
{
    try {

        if ((request.getPath().getDepth() < 1) || (request.getPath().getEntry(0) != endpointIdentifier)) {
            finished.emit(RequestHandleResult::Error, request);
        } else if (request.getType() == RequestType::Get) {
            handleGetRequest(request);
        } else if (request.getType() == RequestType::Delete) {
            handleDeleteRequest(request);
        }
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to emit finished signal, already emitting. Error: {}", e.what());
    }
}

void SessionEndpoint::handleGetRequest(RestRequest& request) noexcept
{
    try {
        if (request.getPath().getDepth() == 1) {
            auto responsebody = nlohmann::ordered_json{};
            responsebody["count"] = mDb.getSessionCount();
            request.setReturnBody(responsebody.dump());
            finished.emit(RequestHandleResult::Ok, request);
        } else if (request.getPath().getDepth() == 3) {
            auto sessionId = getSessionIndex(request.getPath().getEntry(1).value_or(""));
            if (not sessionId.has_value()) {
                finished.emit(RequestHandleResult::Error, request);
            }
            if (request.getPath().getEntry(2) == "data") {
                auto asyncResult =
                    mDb.getSessionByIndexAsync(sessionId.value()); // NOLINT(bugprone-unchecked-optional-access)
                handleSessionGetRequest<GetSessionRequest,
                                        Storage::GetSessionResult,
                                        Common::SessionData,
                                        SessionDataCache>(asyncResult, request, mGetSessionRequests);
            } else if (request.getPath().getEntry(2) == "metadata") {
                auto asyncResult =
                    mDb.getSessionMetaDataByIndexAsync(sessionId.value()); // NOLINT(bugprone-unchecked-optional-access)
                handleSessionGetRequest<GetSessionMetadataRequest,
                                        Storage::GetSessionMetaDataResult,
                                        Common::SessionMetaData,
                                        SessionMetadataCache>(asyncResult, request, mGetSessionMetadataRequests);
            }
        }
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to emit finished signal, already emitting. Error: {}", e.what());
    }
}

void SessionEndpoint::handleDeleteRequest(RestRequest& request) noexcept
{
    try {
        if (request.getPath().getDepth() == 2) {
            auto const sessionId = getSessionIndex(request.getPath().getEntry(1).value_or(""));
            if (not sessionId.has_value()) {
                finished.emit(RequestHandleResult::Error, request);
            }
            mDb.deleteSession(sessionId.value()); // NOLINT(bugprone-unchecked-optional-access)
            finished.emit(RequestHandleResult::Ok, request);
            return;
        }
        finished.emit(RequestHandleResult::Error, request);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to emit finished signal, already emitting. Error: {}", e.what());
    }
}

void SessionEndpoint::logError(std::string const& logMsg)
{
    SPDLOG_ERROR(logMsg);
}

namespace
{
std::optional<std::size_t> getSessionIndex(std::string_view index) noexcept
{
    auto sessionIndex = std::size_t{0};
    auto const result = std::from_chars(index.cbegin(), index.cend(), sessionIndex);
    if (result.ec == std::errc::invalid_argument) {
        return std::nullopt;
    }
    return sessionIndex;
}
} // namespace
} // namespace Rapid::Rest
