// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionManagementWorkflow.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::Workflow::Qt
{

RestSessionManagementWorkflow::RestSessionManagementWorkflow(Rest::IRestClient& client)
    : Rapid::Workflow::RestSessionManagementWorkflow{client}
    , mProvider{std::make_shared<Common::Qt::SessionMetadataProvider>()}
{
    std::ignore =
        sessionMetadataDownloadFinished.connect(&RestSessionManagementWorkflow::onSessionMetadataDownloaded, this);
}

RestSessionManagementWorkflow::~RestSessionManagementWorkflow() = default;

std::shared_ptr<Common::Qt::SessionMetadataProvider> RestSessionManagementWorkflow::getProvider() const noexcept
{
    return mProvider;
}

void RestSessionManagementWorkflow::onSessionMetadataDownloaded(std::size_t index, DownloadResult result)
{
    if (result != DownloadResult::Ok) {
        return;
    }
    auto const maybeData = getSessionMetadata(index);
    if (not maybeData.has_value()) {
        return;
    }
    auto session = maybeData.value();
    auto maybeStoreSession = mProvider->getItem(qsizetype(index));
    if (maybeStoreSession.has_value() and maybeStoreSession.value() == session) {
        mProvider->updateItem(qsizetype(index), maybeData.value());
        SPDLOG_INFO("Update session metadata in session metadata provider at index {}", index);
    } else {
        mProvider->addItemAt(static_cast<qsizetype>(index), maybeData.value());
        SPDLOG_INFO("Inserted session metadata in session metadata provider at index {}", index);
    }
}

} // namespace Rapid::Workflow::Qt
