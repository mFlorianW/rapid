// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LapDataProvider.hpp"
#include <ranges>

namespace Rapid::Common::Qt
{

LapDataProvider::LapDataProvider(TrackData const& track, std::vector<LapData> laps)
    : TableModelDataProvider<LapData>{createColumnNames(track), laps}
{
    setDataExtractor([this](LapData const& item, qsizetype column, qint32 role) -> QVariant {
        if (role == ::Qt::DisplayRole && column < getColumnCount()) {
            if (column == 0) {
                return QString::fromStdString(item.getLaptime().asString());
            } else {
                auto const maybeLaptime = item.getSectorTime(column - 1);
                if (maybeLaptime.has_value()) {
                    return QString::fromStdString(maybeLaptime->asString());
                }
            }
        }
        return QVariant{};
    });
}

QStringList LapDataProvider::createColumnNames(TrackData const& track) const noexcept
{
    auto columns = QStringList{"Lap time"};
    auto sectorCount = track.getSections().size();
    if (sectorCount > 1) {
        for (auto const& index : std::views::iota(0, static_cast<int>(sectorCount + 1))) {
            columns << QString{"Section%1"}.arg(QString::number(index));
        }
    }
    return columns;
}

} // namespace Rapid::Common::Qt
