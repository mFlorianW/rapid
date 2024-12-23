// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionEndpoint.hpp"
#include <JsonSerializer.hpp>
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
            finished.emit(RequestHandleResult::Error, request);
        } else if (request.getPath().getDepth() == 2) {
            auto sessionId = getSessionIndex(request.getPath().getEntry(1).value_or(""));
            if (not sessionId.has_value()) {
                finished.emit(RequestHandleResult::Error, request);
            }
            auto const asyncResult =
                mDb.getSessionByIndexAsync(sessionId.value()); // NOLINT(bugprone-unchecked-optional-access)
            mGetSessionRequests.insert(
                {asyncResult.get(), SessionGetDataRequest{.sessionResult = asyncResult, .request = request}});
            if (asyncResult->getResult() == System::Result::Ok) {
                onSessionResult(asyncResult.get());
            } else if (asyncResult->getResult() == System::Result::Error) {
                finished.emit(RequestHandleResult::Error, request);
            } else {
                std::ignore = asyncResult->done.connect([this](auto* result) {
                    onSessionResult(result);
                });
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
        }
        finished.emit(RequestHandleResult::Error, request);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to emit finished signal, already emitting. Error: {}", e.what());
    }
}

void SessionEndpoint::onSessionResult(System::AsyncResult* result)
{
    if (mGetSessionRequests.count(result) < 1) {
        SPDLOG_ERROR("Session database result handler called without request.");
    }
    auto& getRequest = mGetSessionRequests.at(result);
    auto maybeSession = getRequest.sessionResult->getResultValue();
    if (getRequest.sessionResult->getResult() == System::Result::Ok && maybeSession.has_value()) {
        auto session = maybeSession.value();
        auto rawBody = Common::JsonSerializer::Session::serialize(session);
        getRequest.request.setReturnBody(rawBody);
        finished.emit(RequestHandleResult::Ok, getRequest.request);
    } else {
        finished.emit(RequestHandleResult::Error, getRequest.request);
    }
    mGetSessionRequests.erase(result);
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
