// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LocalSessionManagement.hpp"
#include <ranges>
#include <spdlog/spdlog.h>

using namespace Rapid::Common::Qt;

namespace Rapid::Workflow::Qt
{

LocalSessionManagement::LocalSessionManagement(Storage::ISessionDatabase* db)
    : QObject{}
    , Workflow::LocalSessionManagement{db}
    , mMetaDataListModel{std::make_unique<SessionMetaDataListModel>()}
{
    for (auto const& index : std::ranges::iota_view{std::size_t{0}, mDb->getSessionCount()}) {
        auto task = mDb->getSessionMetaDataByIndexAsync(index);
        if (task->getResult() == System::Result::Ok) {
            mMetaDataListModel->insertItem(index, task->getResultValue().value_or(Common::SessionMetaData{}));
        } else {
            auto con = task->done.connect([this](auto* result) {
                handleSessionMetaDataResult(result);
            });
            mPendingResults.insert(
                {task.get(), PendingResult{.index = index, .result = task, .doneConnection = std::move(con)}});
        }
    }
}

LocalSessionManagement::~LocalSessionManagement() = default;

Rapid::Common::Qt::SessionMetaDataListModel* LocalSessionManagement::getSessionMetaDataListModel() const noexcept
{
    return mMetaDataListModel.get();
}

void LocalSessionManagement::handleSessionMetaDataResult(System::AsyncResult* result)
{
    if (not mPendingResults.contains(result)) {
        SPDLOG_ERROR("Handle meta data result is called with a invalid result.");
        return;
    }
    auto const& pendingResult = mPendingResults.at(result);
    auto const maybeResult = pendingResult.result->getResultValue();
    if (maybeResult.has_value()) {
        mMetaDataListModel->insertItem(pendingResult.index, maybeResult.value());
    }
    mPendingResults.erase(result);
}

} // namespace Rapid::Workflow::Qt
