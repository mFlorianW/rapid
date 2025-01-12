// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionMetaDataProvider.hpp"
#include <spdlog/spdlog.h>
#include <system/EventLoop.hpp>

namespace Rapid::Storage::Qt
{

SessionMetaDataProvider::SessionMetaDataProvider(ISessionDatabase& sessionDb)
    : Rapid::Common::Qt::SessionMetadataProvider{}
    , mSessionDb{sessionDb}
{
    auto sessionCount = mSessionDb.getSessionCount();
    for (std::size_t i = 0; i < sessionCount; i++) {
        requestSessionMetaData(i);
    }

    auto conEval = System::EventLoop::instance().getConnectionEvaluator();
    std::ignore = mSessionDb.sessionAdded.connectDeferred(conEval, [this](auto index) {
        requestSessionMetaData(index);
    });
}

void SessionMetaDataProvider::requestSessionMetaData(std::size_t index) noexcept
{
    auto result = mSessionDb.getSessionMetaDataByIndexAsync(index);
    if (result->getResult() == System::Result::Ok) {
        addItem(result->getResultValue().value_or(Common::SessionMetaData{}));
    } else {
        mRequestCache.insert({result.get(), result});
        std::ignore = result->done.connect([this, index](auto* self) {
            handleSessionMetaDataRequest(self, index);
        });
    }
}

void SessionMetaDataProvider::handleSessionMetaDataRequest(System::AsyncResult* self, size_t index) noexcept
{
    if (not mRequestCache.contains(self)) {
        SPDLOG_WARN("SessionMetaData request handler called with unknown result. {}", fmt::ptr(self));
        return;
    }
    auto result = mRequestCache.at(self);
    if (result->getResult() == Rapid::System::Result::Ok) {
        addItem(result->getResultValue().value_or(Common::SessionMetaData{}));
    } else {
        SPDLOG_ERROR("Failed to request session meta data for index {}", index);
    }
}

} // namespace Rapid::Storage::Qt
