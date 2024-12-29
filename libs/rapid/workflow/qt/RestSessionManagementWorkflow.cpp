// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionManagementWorkflow.hpp"

namespace Rapid::Workflow::Qt
{

RestSessionManagementWorkflow::RestSessionManagementWorkflow(Rest::IRestClient& client)
    : Rapid::Workflow::RestSessionManagementWorkflow{client}
    , mProvider{std::make_shared<Common::Qt::SessionMetadataProvider>()}
{
    std::ignore =
        sessionMetadataDownloadFinshed.connect(&RestSessionManagementWorkflow::onSessionMetadataDownloaded, this);
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
    auto const maybeData = getSessionMetadata(0);
    if (maybeData.has_value()) {
        mProvider->addItemAt(static_cast<qsizetype>(index), maybeData.value());
    }
}

} // namespace Rapid::Workflow::Qt
