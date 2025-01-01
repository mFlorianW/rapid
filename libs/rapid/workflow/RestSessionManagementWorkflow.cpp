// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionManagementWorkflow.hpp"
#include <common/JsonDeserializer.hpp>
#include <nlohmann/json.hpp>
#include <ranges>
#include <spdlog/spdlog.h>
#include <sstream>

namespace Rapid::Workflow
{

RestSessionManagementWorkflow::RestSessionManagementWorkflow(Rest::IRestClient& restClient) noexcept
    : IRestSessionManagementWorkflow()
    , mRestClient{restClient}
{
}

std::size_t RestSessionManagementWorkflow::getSessionCount() const noexcept
{
    return mSessionCount;
}

void RestSessionManagementWorkflow::fetchSessionCount() noexcept
{
    auto call = mRestClient.execute(Rest::RestRequest{Rest::RequestType::Get, "/sessions"});
    mFetchCounterCache.insert({call.get(), call});
    if (call->isFinished()) {
        onFetchSessionCountFinished(call.get());
    } else {
        std::ignore = call->finished.connect(&RestSessionManagementWorkflow::onFetchSessionCountFinished, this);
    }
}

std::optional<Common::SessionData> RestSessionManagementWorkflow::getSession(std::size_t index) const noexcept
{
    if (mDownloadedSessions.count(index) == 0) {
        return std::nullopt;
    }
    return mDownloadedSessions.at(index);
}

std::optional<Common::SessionMetaData> RestSessionManagementWorkflow::getSessionMetadata(
    std::size_t index) const noexcept
{
    if (mDownloadedSessionMetadata.count(index) == 0) {
        return std::nullopt;
    }
    return mDownloadedSessionMetadata.at(index);
}

void RestSessionManagementWorkflow::downloadSession(std::size_t index) noexcept
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

void RestSessionManagementWorkflow::downloadSessionMetadata(std::size_t index) noexcept
{
    std::ostringstream outStream;
    outStream << "/sessions/" << index << "/metadata";
    download(outStream.str(), index, mSessionMetadataDownloadCache, [this](auto&& call) {
        onDownloadFinished(call,
                           mSessionMetadataDownloadCache,
                           mDownloadedSessionMetadata,
                           sessionMetadataDownloadFinished,
                           &Common::JsonDeserializer::SessionMetaData::deserialize);
    });
}

void RestSessionManagementWorkflow::downloadAllSessionMetadata() noexcept
{
    auto call = mRestClient.execute(Rest::RestRequest{Rest::RequestType::Get, "/sessions"});
    mFetchCounterCache.insert({call.get(), call});
    if (call->isFinished()) {
        onFetchSessionCountAllDownloadFinished(call.get());
    } else {
        std::ignore =
            call->finished.connect(&RestSessionManagementWorkflow::onFetchSessionCountAllDownloadFinished, this);
    }
}

void RestSessionManagementWorkflow::onFetchSessionCountFinished(Rest::RestCall* call) noexcept
{
    try {
        if (mFetchCounterCache.count(call) == 0) {
            SPDLOG_ERROR("Download handler called with invalid call.");
        }
        auto dlResult = getDownloadResult(*call);
        auto const maybeCount = parseSessionCountDownload(*call);
        if (maybeCount.has_value() and dlResult == DownloadResult::Ok) {
            mSessionCount = maybeCount.value();
        }
        sessionCountFetched.emit(dlResult);
        mFetchCounterCache.erase(call);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to fetch session count. Error: {}", e.what());
    }
}

void RestSessionManagementWorkflow::onFetchSessionCountAllDownloadFinished(Rest::RestCall* call) noexcept
{
    try {

        if (mFetchCounterCache.count(call) == 0) {
            SPDLOG_ERROR("Download handler called with invalid call.");
        }
        auto dlResult = getDownloadResult(*call);
        auto maybeCount = parseSessionCountDownload(*call);
        if (maybeCount.has_value() and dlResult == DownloadResult::Ok) {
            auto sessions = maybeCount.value();
            for (auto const& index : std::views::iota(0U, sessions)) {
                downloadSessionMetadata(index);
            }
        }
        mFetchCounterCache.erase(call);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to fetch session count of all session metadata download. Error: {}", e.what());
    }
}

void RestSessionManagementWorkflow::logError(std::string const& errorMsg) const noexcept
{
    SPDLOG_ERROR(errorMsg);
}

std::optional<std::size_t> RestSessionManagementWorkflow::parseSessionCountDownload(Rest::RestCall& call) noexcept
{
    try {
        auto jsonDoc = nlohmann::ordered_json{}.parse(call.getData());
        return jsonDoc["count"];
    } catch (std::exception const& e) {
        SPDLOG_ERROR("RestSessionDownloader fetchSessionCount Error: DeserializeJson failed: {}", e.what());
    }
    return std::nullopt;
}

DownloadResult RestSessionManagementWorkflow::getDownloadResult(Rest::RestCall const& call) noexcept
{
    auto const dlResult =
        call.getResult() == Rest::RestCallResult::Success ? DownloadResult::Ok : DownloadResult::Error;
    return dlResult;
}

} // namespace Rapid::Workflow
