// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionEndpoint.hpp"
#include <ArduinoJson.hpp>
#include <JsonSerializer.hpp>
#include <charconv>

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

RequestHandleResult SessionEndpoint::handleRestRequest(RestRequest& request) noexcept
{
    if ((request.getPath().getDepth() < 1) || (request.getPath().getEntry(0) != endpointIdentifier)) {
        return RequestHandleResult::Error;
    } else if (request.getType() == RequestType::Get) {
        return handleGetRequest(request);
    } else if (request.getType() == RequestType::Delete) {
        return handleDeleteRequest(request);
    }

    return RequestHandleResult::Error;
}

RequestHandleResult SessionEndpoint::handleGetRequest(RestRequest& request) noexcept
{
    if (request.getPath().getDepth() == 1) {
        auto responsebody = ArduinoJson::JsonDocument{};
        responsebody["count"] = mDb.getSessionCount();
        auto rawBody = std::string{};
        ArduinoJson::serializeJson(responsebody, rawBody);
        request.setReturnBody(rawBody);
        return RequestHandleResult::Ok;
    } else if (request.getPath().getDepth() == 2) {
        auto sessionId = getSessionIndex(request.getPath().getEntry(1).value_or(""));
        if (!sessionId.has_value()) {
            return RequestHandleResult::Error;
        }

        auto const session = mDb.getSessionByIndex(sessionId.value());
        if (!session.has_value()) {
            return RequestHandleResult::Error;
        }

        auto responsebody = ArduinoJson::JsonDocument{};
        auto jsonRoot = responsebody.to<ArduinoJson::JsonObject>();
        Common::JsonSerializer::serializeSessionData(session.value(), jsonRoot);
        auto rawBody = std::string{};
        ArduinoJson::serializeJson(responsebody, rawBody);
        request.setReturnBody(rawBody);
        return RequestHandleResult::Ok;
    }

    return RequestHandleResult::Error;
}

RequestHandleResult SessionEndpoint::handleDeleteRequest(RestRequest& request) noexcept
{
    if (request.getPath().getDepth() == 2) {
        auto const sessionId = getSessionIndex(request.getPath().getEntry(1).value_or(""));
        if (!sessionId.has_value()) {
            return RequestHandleResult::Error;
        }
        mDb.deleteSession(sessionId.value());
        return RequestHandleResult::Ok;
    }
    return RequestHandleResult::Error;
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
