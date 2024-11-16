// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionDownloader.hpp"
#include <ArduinoJson.hpp>
#include <JsonDeserializer.hpp>
#include <spdlog/spdlog.h>
#include <sstream>

namespace Rapid::Workflow
{

RestSessionDownloader::RestSessionDownloader(Rest::IRestClient& restClient) noexcept
    : ISessionDownloader()
    , mRestClient{restClient}
{
}

std::size_t RestSessionDownloader::getSessionCount() const noexcept
{
    return mSessionCount;
}

void RestSessionDownloader::fetchSessionCount() noexcept
{
    auto call = mRestClient.execute(Rest::RestRequest{Rest::RequestType::Get, "/sessions"});
    mFetchCounterCache.insert({call.get(), call});
    if (call->isFinished()) {
        onFetchSessionCountFinished(call.get());
    } else {
        call->finished.connect(&RestSessionDownloader::onFetchSessionCountFinished, this);
    }
}

std::optional<Common::SessionData> RestSessionDownloader::getSession(std::size_t index) const noexcept
{
    if (mDownloadedSessions.count(index) == 0) {
        return std::nullopt;
    }
    return mDownloadedSessions.at(index);
}

void RestSessionDownloader::downloadSession(std::size_t index) noexcept
{
    std::ostringstream outStream;
    outStream << "/sessions/" << index;
    auto call = mRestClient.execute(Rest::RestRequest{Rest::RequestType::Get, outStream.str()});
    mDownloadSessionCache.insert({call.get(), {.index = index, .call = call}});
    if (call->isFinished()) {
        onSessionDownloadFinished(call.get());
    } else {
        call->finished.connect(&RestSessionDownloader::onSessionDownloadFinished, this);
    }
}

void RestSessionDownloader::onFetchSessionCountFinished(Rest::RestCall* call) noexcept
{
    if (mFetchCounterCache.count(call) > 0) {
        auto const dlResult =
            call->getResult() == Rest::RestCallResult::Success ? DownloadResult::Ok : DownloadResult::Error;
        if (dlResult == DownloadResult::Ok) {
            auto jsonDoc = ArduinoJson::DynamicJsonDocument{256};
            auto const error = ArduinoJson::deserializeJson(jsonDoc, call->getData());
            if (error != ArduinoJson::DeserializationError::Ok) {
                spdlog::error("RestSessionDownloader fetchSessionCount Error: DeserializeJson failed: {}",
                              error.c_str());
            }
            mSessionCount = jsonDoc["count"].as<std::size_t>();
        }
        sessionCountFetched.emit(dlResult);
        mFetchCounterCache.erase(call);
    }
}

void RestSessionDownloader::onSessionDownloadFinished(Rest::RestCall* call) noexcept
{
    if (mDownloadSessionCache.size() > 0 && call != nullptr) {
        auto const dlResult =
            call->getResult() == Rest::RestCallResult::Success ? DownloadResult::Ok : DownloadResult::Error;
        auto const index = mDownloadSessionCache.at(call).index;
        if (dlResult == DownloadResult::Ok) {
            auto session = Common::JsonDeserializer::deserializeSessionData(call->getData());
            if (!session) {
                spdlog::error("RestSessionDownloader downloadSessionError: DeserializeJson failed.");
            } else {
                mDownloadedSessions.insert({index, session.value()});
            }
        }
        sessionDownloadFinshed.emit(index, dlResult);
        mDownloadSessionCache.erase(call);
    }
}

} // namespace Rapid::Workflow
