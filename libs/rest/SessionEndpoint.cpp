// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionEndpoint.hpp"
#include <JsonSerializer.hpp>
#include <charconv>
#include <nlohmann/json.hpp>

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
    if ((request.getPath().getDepth() < 1) || (request.getPath().getEntry(0) != endpointIdentifier)) {
        finished.emit(RequestHandleResult::Error, request);
    } else if (request.getType() == RequestType::Get) {
        handleGetRequest(request);
    } else if (request.getType() == RequestType::Delete) {
        handleDeleteRequest(request);
    }
}

void SessionEndpoint::handleGetRequest(RestRequest& request) noexcept
{
    if (request.getPath().getDepth() == 1) {
        auto responsebody = nlohmann::ordered_json{};
        responsebody["count"] = mDb.getSessionCount();
        request.setReturnBody(responsebody.dump());
        finished.emit(RequestHandleResult::Error, request);
    } else if (request.getPath().getDepth() == 2) {
        auto sessionId = getSessionIndex(request.getPath().getEntry(1).value_or(""));
        if (!sessionId.has_value()) {
            finished.emit(RequestHandleResult::Error, request);
        }

        auto const session = mDb.getSessionByIndex(sessionId.value());
        if (!session.has_value()) {
            finished.emit(RequestHandleResult::Error, request);
        }

        auto rawBody = Common::JsonSerializer::Session::serialize(session.value());
        request.setReturnBody(rawBody);
        finished.emit(RequestHandleResult::Ok, request);
    }
}

void SessionEndpoint::handleDeleteRequest(RestRequest& request) noexcept
{
    if (request.getPath().getDepth() == 2) {
        auto const sessionId = getSessionIndex(request.getPath().getEntry(1).value_or(""));
        if (!sessionId.has_value()) {
            finished.emit(RequestHandleResult::Error, request);
        }
        mDb.deleteSession(sessionId.value());
        finished.emit(RequestHandleResult::Ok, request);
    }
    finished.emit(RequestHandleResult::Error, request);
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
