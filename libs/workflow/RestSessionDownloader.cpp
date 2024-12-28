// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionDownloader.hpp"
#include <common/JsonDeserializer.hpp>
#include <nlohmann/json.hpp>
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
        std::ignore = call->finished.connect(&RestSessionDownloader::onFetchSessionCountFinished, this);
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
    outStream << "/sessions/" << index << "/data";
    auto call = mRestClient.execute(Rest::RestRequest{Rest::RequestType::Get, outStream.str()});
    mDownloadSessionCache.insert({call.get(), {.index = index, .call = call}});
    if (call->isFinished()) {
        onSessionDownloadFinished(call.get());
    } else {
        std::ignore = call->finished.connect(&RestSessionDownloader::onSessionDownloadFinished, this);
    }
}

void RestSessionDownloader::onFetchSessionCountFinished(Rest::RestCall* call) noexcept
{
    if (mFetchCounterCache.count(call) > 0) {
        auto const dlResult =
            call->getResult() == Rest::RestCallResult::Success ? DownloadResult::Ok : DownloadResult::Error;
        if (dlResult == DownloadResult::Ok) {
            try {
                auto jsonDoc = nlohmann::ordered_json{}.parse(call->getData());
                mSessionCount = jsonDoc["count"];
            } catch (std::exception const& e) {
                SPDLOG_ERROR("RestSessionDownloader fetchSessionCount Error: DeserializeJson failed: {}", e.what());
            }
        }
        try {
            sessionCountFetched.emit(dlResult);
        } catch (std::exception const& e) {
            SPDLOG_ERROR("Failed ot emit sessionCountFetched. Error already emitting.");
        }
    }
    mFetchCounterCache.erase(call);
}

void RestSessionDownloader::onSessionDownloadFinished(Rest::RestCall* call) noexcept
{
    if (mDownloadSessionCache.size() > 0 && call != nullptr) {
        auto const dlResult =
            call->getResult() == Rest::RestCallResult::Success ? DownloadResult::Ok : DownloadResult::Error;
        auto const index = mDownloadSessionCache.at(call).index;
        if (dlResult == DownloadResult::Ok) {
            auto session = Common::JsonDeserializer::Session::deserialize(call->getData());
            if (!session) {
                spdlog::error("RestSessionDownloader downloadSessionError: DeserializeJson failed.");
            } else {
                mDownloadedSessions.insert({index, session.value()});
            }
        }
        try {
            sessionDownloadFinshed.emit(index, dlResult);
        } catch (std::exception const& e) {
            SPDLOG_ERROR("Failed ot emit sessionCountFetched. Error already emitting.");
        }
        mDownloadSessionCache.erase(call);
    }
}

} // namespace Rapid::Workflow
