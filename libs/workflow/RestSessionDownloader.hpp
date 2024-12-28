// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "ISessionDownloader.hpp"
#include "rest/IRestClient.hpp"

namespace Rapid::Workflow
{
class RestSessionDownloader final : public ISessionDownloader
{
public:
    RestSessionDownloader(Rest::IRestClient& restClient) noexcept;

    /**
     * Default empty destructor
     */
    ~RestSessionDownloader() noexcept override = default;

    /**
     * Deleted copy constructor
     */
    RestSessionDownloader(RestSessionDownloader const&) = delete;

    /**
     * Deleted copy operator
     */
    RestSessionDownloader& operator=(RestSessionDownloader const&) = delete;

    /**
     * Deleted move constructor
     */
    RestSessionDownloader(RestSessionDownloader&&) = delete;

    /**
     * Deleted move operator
     */
    RestSessionDownloader& operator=(RestSessionDownloader&&) = delete;

    /**
     * @copydoc ISessionDownloader::getSession()
     */
    std::size_t getSessionCount() const noexcept override;

    /**
     * @copydoc ISessionDownloader::featchSessionCount()
     */
    void fetchSessionCount() noexcept override;

    /**
     * @copydoc ISessionDownloader::getSession()
     */
    std::optional<Common::SessionData> getSession(std::size_t index) const noexcept override;

    /**
     * @copydoc ISessionDownloader::getSessionMetadata()
     */
    std::optional<Common::SessionMetaData> getSessionMetadata(std::size_t index) const noexcept override;

    /**
     * @copydoc ISessionDownloader::downloadSession()
     */
    void downloadSession(std::size_t index) noexcept override;

    /**
     * @copydoc ISessionDownloader::downloadSessionMetadata
     */
    void downloadSessionMetadata(std::size_t index) noexcept override;

private:
    void onFetchSessionCountFinished(Rest::RestCall* call) noexcept;
    void onSessionDownloadFinished(Rest::RestCall* call) noexcept;
    void onSessionMetadataDownloadFinished(Rest::RestCall* call) noexcept;

private:
    struct SessionDownloadCacheEntry
    {
        std::size_t index{0};
        std::shared_ptr<Rest::RestCall> call;
    };

    Rest::IRestClient& mRestClient;
    std::size_t mSessionCount{0};
    std::unordered_map<Rest::RestCall*, std::shared_ptr<Rest::RestCall>> mFetchCounterCache;
    std::unordered_map<Rest::RestCall*, SessionDownloadCacheEntry> mDownloadSessionCache;
    std::unordered_map<Rest::RestCall*, SessionDownloadCacheEntry> mSessionMetadataDownloadCache;
    std::unordered_map<std::size_t, Common::SessionData> mDownloadedSessions;
    std::unordered_map<std::size_t, Common::SessionMetaData> mDownloadedSessionMetadata;
};

} // namespace Rapid::Workflow
