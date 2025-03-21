// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionMetaDataSortListModel.hpp"
#include "SessionMetaDataListModel.hpp"

#include <spdlog/spdlog.h>

namespace Rapid::Common::Qt
{

SessionMetaDataSortListModel::SessionMetaDataSortListModel()
{
    sort(0, ::Qt::SortOrder::DescendingOrder);
};

SessionMetaDataSortListModel::~SessionMetaDataSortListModel() = default;

bool SessionMetaDataSortListModel::lessThan(QModelIndex const& sourceLeft, QModelIndex const& sourceRight) const
{
    auto const leftSessionDate = sourceModel()->data(sourceLeft, SessionMetaDataListModel::Date).toString();
    auto const leftSessionTime = sourceModel()->data(sourceLeft, SessionMetaDataListModel::Time).toString();
    auto const rightSessionDate = sourceModel()->data(sourceRight, SessionMetaDataListModel::Date).toString();
    auto const rightSessionTime = sourceModel()->data(sourceRight, SessionMetaDataListModel::Time).toString();

    auto const leftDate = Common::Date{leftSessionDate.toStdString()};
    auto const rightDate = Common::Date{rightSessionDate.toStdString()};
    auto const leftTime = Common::Timestamp{leftSessionTime.toStdString()};
    auto const rightTime = Common::Timestamp{rightSessionTime.toStdString()};

    if (leftDate != rightDate) {
        return leftDate < rightDate;
    } else if (leftTime != rightTime) {
        return leftTime < rightTime;
    }
    return false;
}

} // namespace Rapid::Common::Qt
