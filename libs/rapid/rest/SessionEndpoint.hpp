// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IRestRequestHandler.hpp"
#include "storage/ISessionDatabase.hpp"
#include <common/JsonSerializer.hpp>

namespace Rapid::Rest
{

class SessionEndpoint : public IRestRequestHandler
{
public:
    SessionEndpoint(Storage::ISessionDatabase& database) noexcept;

    void handleRestRequest(RestRequest& request) noexcept override;

private:
    void handleGetRequest(RestRequest& request) noexcept;
    void handleDeleteRequest(RestRequest& request) noexcept;

    template <typename CacheTyp, typename AsyncResult, typename ResultType, typename Cache>
    void handleSessionGetRequest(std::shared_ptr<AsyncResult>& asyncResult, RestRequest& request, Cache& cache)
    {
        cache.insert({asyncResult.get(), CacheTyp{.sessionResult = asyncResult, .request = request}});
        if (asyncResult->getResult() == System::Result::Ok) {
            onGetResult<ResultType, Cache>(asyncResult.get(), cache);
        } else if (asyncResult->getResult() == System::Result::Error) {
            finished.emit(RequestHandleResult::Error, request);
        } else {
            std::ignore = asyncResult->done.connect([this, &cache](auto* result) {
                this->onGetResult<ResultType, Cache>(result, cache);
            });
        }
    }

    template <typename T, typename Cache>
    void onGetResult(System::AsyncResult* result, Cache& cache)
    {
        if (cache.count(result) < 1) {
            logError("Get result handler called without request");
        }
        auto& getRequest = cache.at(result);
        std::optional<T> maybeResult = getRequest.sessionResult->getResultValue();
        if (getRequest.sessionResult->getResult() == System::Result::Ok && maybeResult.has_value()) {
            auto rawBody = Common::JsonSerializer::Session::serialize(maybeResult.value());
            getRequest.request.setReturnBody(rawBody);
            finished.emit(RequestHandleResult::Ok, getRequest.request);
        } else {
            finished.emit(RequestHandleResult::Error, getRequest.request);
        }
        mGetSessionRequests.erase(result);
    }

    void logError(std::string const& logMsg);

private:
    Storage::ISessionDatabase& mDb;

    template <typename T>
    struct GenericAsyncRequest
    {
        std::shared_ptr<T> sessionResult;
        RestRequest request;
    };
    using GetSessionRequest = GenericAsyncRequest<Storage::GetSessionResult>;
    using GetSessionMetadataRequest = GenericAsyncRequest<Storage::GetSessionMetaDataResult>;

    using SessionDataCache = std::unordered_map<System::AsyncResult*, GetSessionRequest>;
    SessionDataCache mGetSessionRequests;
    using SessionMetadataCache = std::unordered_map<System::AsyncResult*, GetSessionMetadataRequest>;
    SessionMetadataCache mGetSessionMetadataRequests;
};

} // namespace Rapid::Rest
