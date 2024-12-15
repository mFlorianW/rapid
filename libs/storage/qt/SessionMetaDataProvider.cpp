// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionMetaDataProvider.hpp"
#include <EventLoop.hpp>
#include <spdlog/spdlog.h>

namespace Rapid::Storage::Qt
{

SessionMetaDataProvider::SessionMetaDataProvider(ISessionDatabase& sessionDb)
    : Rapid::Common::Qt::TableModelDataProvider<Common::SessionMetaData>{{tr("Track"), tr("Date"), tr("Time")}}
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

    setDataExtractor([](Common::SessionMetaData const& item, std::size_t column, qint32 role) {
        auto data = QVariant{};
        if (role == ::Qt::DisplayRole) {
            switch (column) {
            case 0:
                data = QString::fromStdString(item.getTrack().getTrackName());
                break;
            case 1:
                data = QString::fromStdString(item.getSessionDate().asString());
                break;
            case 2:
                data = QString::fromStdString(item.getSessionTime().asString());
                break;
            default:
                return QVariant{};
            }
        }
        return data;
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
