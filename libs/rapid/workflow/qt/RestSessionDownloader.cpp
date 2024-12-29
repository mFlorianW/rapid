// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionDownloader.hpp"
#include <QTranslator>

namespace Rapid::Workflow::Qt
{

RestSessionDownloader::RestSessionDownloader(Rest::IRestClient& client)
    : Rapid::Workflow::RestSessionDownloader{client}
    , mProvider{std::make_shared<Common::Qt::SessionMetadataProvider>()}
{
    std::ignore = sessionMetadataDownloadFinshed.connect(&RestSessionDownloader::onSessionMetadataDownloaded, this);
}

RestSessionDownloader::~RestSessionDownloader() = default;

std::shared_ptr<Common::Qt::SessionMetadataProvider> RestSessionDownloader::getProvider() const noexcept
{
    return mProvider;
}

void RestSessionDownloader::onSessionMetadataDownloaded(std::size_t index, DownloadResult result)
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
