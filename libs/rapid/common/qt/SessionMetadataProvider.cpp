// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionMetadataProvider.hpp"

namespace Rapid::Common::Qt
{

SessionMetadataProvider::SessionMetadataProvider()
    : Rapid::Common::Qt::TableModelDataProvider<Common::SessionMetaData>{{tr("Date"), tr("Time"), tr("Track")}}
{
    setDataExtractor([](Common::SessionMetaData const& item, std::size_t column, qint32 role) {
        auto data = QVariant{};
        if (role == ::Qt::DisplayRole) {
            switch (column) {
            case 0:
                data = QString::fromStdString(item.getSessionDate().asString());
                break;
            case 1:
                data = QString::fromStdString(item.getSessionTime().asString());
                break;
            case 2:
                data = QString::fromStdString(item.getTrack().getTrackName());
                break;
            default:
                return QVariant{};
            }
        }
        return data;
    });
}

} // namespace Rapid::Common::Qt
