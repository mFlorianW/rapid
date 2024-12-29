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

std::optional<Common::SessionMetaData> RestSessionDownloader::getSessionMetadata(std::size_t index) const noexcept
{
    if (mDownloadedSessionMetadata.count(index) == 0) {
        return std::nullopt;
    }
    return mDownloadedSessionMetadata.at(index);
}

void RestSessionDownloader::downloadSession(std::size_t index) noexcept
{
    std::ostringstream outStream;
    outStream << "/sessions/" << index << "/data";
    download(outStream.str(), index, mDownloadSessionCache, [this](auto&& call) {
        onDownloadFinished(call,
                           mDownloadSessionCache,
                           mDownloadedSessions,
                           sessionDownloadFinshed,
                           &Common::JsonDeserializer::Session::deserialize);
    });
}

void RestSessionDownloader::downloadSessionMetadata(std::size_t index) noexcept
{
    std::ostringstream outStream;
    outStream << "/sessions/" << index << "/metadata";
    download(outStream.str(), index, mSessionMetadataDownloadCache, [this](auto&& call) {
        onDownloadFinished(call,
                           mSessionMetadataDownloadCache,
                           mDownloadedSessionMetadata,
                           sessionMetadataDownloadFinshed,
                           &Common::JsonDeserializer::SessionMetaData::deserialize);
    });
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

void RestSessionDownloader::logError(std::string const& errorMsg) const noexcept
{
    SPDLOG_ERROR(errorMsg);
}

} // namespace Rapid::Workflow
