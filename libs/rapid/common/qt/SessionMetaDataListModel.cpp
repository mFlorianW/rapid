// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionMetaDataListModel.hpp"

namespace Rapid::Common::Qt
{

SessionMetaDataListModel::SessionMetaDataListModel()
    : Rapid::Common::Qt::GenericListModel<SessionMetaData>{{{SessionMetaDataListModel::TrackName, "trackName"},
                                                            {SessionMetaDataListModel::Time, "time"},
                                                            {SessionMetaDataListModel::Date, "date"}}}
{
}

SessionMetaDataListModel::~SessionMetaDataListModel() = default;

QVariant SessionMetaDataListModel::data(QModelIndex const& index, int role) const noexcept
{
    auto const element = getElement(index.row());
    if (not element.has_value()) {
        return {};
    }
    if (role == TrackName) {
        return QString::fromStdString((*element)->getTrack().getTrackName());
    } else if (role == Date) {
        return QString::fromStdString((*element)->getSessionDate().asString());
    } else if (role == Time) {
        return QString::fromStdString((*element)->getSessionTime().asString());
    }
    return {};
}

} // namespace Rapid::Common::Qt
