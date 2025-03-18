// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionManagementWorkflow.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::Workflow::Qt
{

RestSessionManagementWorkflow::RestSessionManagementWorkflow()
    : mProvider{std::make_shared<Common::Qt::SessionMetadataProvider>()}
    , mListModel{std::make_shared<Common::Qt::SessionMetaDataListModel>()}
{
    mSessionMetadataDownloadConnection =
        sessionMetadataDownloadFinished.connect(&RestSessionManagementWorkflow::onSessionMetadataDownloaded, this);
}

RestSessionManagementWorkflow::RestSessionManagementWorkflow(Rest::IRestClient* client)
    : Rapid::Workflow::RestSessionManagementWorkflow{client}
    , mProvider{std::make_shared<Common::Qt::SessionMetadataProvider>()}
    , mListModel{std::make_shared<Common::Qt::SessionMetaDataListModel>()}
{
    mSessionMetadataDownloadConnection =
        sessionMetadataDownloadFinished.connect(&RestSessionManagementWorkflow::onSessionMetadataDownloaded, this);
}

RestSessionManagementWorkflow::~RestSessionManagementWorkflow() = default;

std::shared_ptr<Common::Qt::SessionMetadataProvider> RestSessionManagementWorkflow::getProvider() const noexcept
{
    return mProvider;
}
std::shared_ptr<Common::Qt::SessionMetaDataListModel> RestSessionManagementWorkflow::getSessionMetadataListModel()
    const noexcept
{
    return mListModel;
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
    auto sessionMetadata = maybeData.value();
    updateProvider(index, sessionMetadata);
    updateListModel(index, sessionMetadata);
}

void RestSessionManagementWorkflow::updateProvider(std::size_t index, Common::SessionMetaData const& sessionMetadata)
{
    auto isSessionStored = mProvider->getItem(qsizetype(index));
    if (isSessionStored.has_value() and isSessionStored.value() == sessionMetadata) {
        mProvider->updateItem(qsizetype(index), sessionMetadata);
        SPDLOG_DEBUG("Update session metadata in session metadata provider at index {}", index);
    } else {
        mProvider->addItemAt(static_cast<qsizetype>(index), sessionMetadata);
        SPDLOG_DEBUG("Inserted session metadata in session metadata provider at index {}", index);
    }
}

void RestSessionManagementWorkflow::updateListModel(std::size_t index, Common::SessionMetaData const& session)
{
    auto const isSessionStoredInListModel = mListModel->getElement(qsizetype(index));
    if (isSessionStoredInListModel.has_value() and isSessionStoredInListModel.value() != nullptr and
        *isSessionStoredInListModel.value() == session) {
        std::ignore = mListModel->updateItem(qsizetype(index), session);
        SPDLOG_DEBUG("Update session metadata in session metadata list model at index {}", index);
    } else {
        std::ignore = mListModel->insertItem(qsizetype(index), session);
        SPDLOG_DEBUG("Inserted session metadata in session metadata list model at index {}", index);
    }
}

} // namespace Rapid::Workflow::Qt
