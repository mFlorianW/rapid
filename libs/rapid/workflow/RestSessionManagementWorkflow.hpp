// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "rest/IRestClient.hpp"
#include "workflow/IRestSessionManagementWorkflow.hpp"

namespace Rapid::Workflow
{
class RestSessionManagementWorkflow : public IRestSessionManagementWorkflow
{
public:
    RestSessionManagementWorkflow(Rest::IRestClient& restClient) noexcept;

    /**
     * Default empty destructor
     */
    ~RestSessionManagementWorkflow() noexcept override = default;

    /**
     * Deleted copy constructor
     */
    RestSessionManagementWorkflow(RestSessionManagementWorkflow const&) = delete;

    /**
     * Deleted copy operator
     */
    RestSessionManagementWorkflow& operator=(RestSessionManagementWorkflow const&) = delete;

    /**
     * Deleted move constructor
     */
    RestSessionManagementWorkflow(RestSessionManagementWorkflow&&) = delete;

    /**
     * Deleted move operator
     */
    RestSessionManagementWorkflow& operator=(RestSessionManagementWorkflow&&) = delete;

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

    /**
     * @copydoc IRestSessionManagementWorkflow::downloadAllSessionMetadata
     */
    void downloadAllSessionMetadata() noexcept override;

private:
    void onFetchSessionCountFinished(Rest::RestCall* call) noexcept;
    void onFetchSessionCountAllDownloadFinished(Rest::RestCall* call) noexcept;

    template <typename Cache>
    void download(std::string const& path,
                  std::size_t index,
                  Cache& cache,
                  std::function<void(Rest::RestCall*)> handler)
    {
        auto call = mRestClient.execute(Rest::RestRequest{Rest::RequestType::Get, path});
        cache.insert({call.get(), {.index = index, .call = call}});
        if (call->isFinished()) {
            handler(call.get());
        } else {
            std::ignore = call->finished.connect(handler);
        }
    }

    template <typename Cache, typename ResultCache, typename SignalType, typename Func>
    void onDownloadFinished(Rest::RestCall* call,
                            Cache& cache,
                            ResultCache& resultCache,
                            SignalType& signal,
                            Func func) noexcept
    {
        if (cache.size() == 0 or call == nullptr) {
            return;
        }
        auto const dlResult = getDownloadResult(*call);
        auto const index = cache.at(call).index;
        if (dlResult == DownloadResult::Ok) {
            auto session = func(call->getData());
            if (!session) {
                logError("Download Failure. Error: JSON deserialization failed.");
            } else {
                resultCache.insert({index, session.value()});
            }
        }
        try {
            signal.emit(index, dlResult);
        } catch (std::exception const& e) {
            logError("Failed to emit download finished. Error already emitting.");
        }
        cache.erase(call);
    }

    void logError(std::string const& errorMsg) const noexcept;
    std::optional<std::size_t> parseSessionCountDownload(Rest::RestCall& call) noexcept;
    DownloadResult getDownloadResult(Rest::RestCall const& call) noexcept;

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
