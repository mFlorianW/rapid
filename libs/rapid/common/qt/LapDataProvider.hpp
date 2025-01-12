// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LAPDATAPROVIDER_HPP
#define LAPDATAPROVIDER_HPP

#include "TableModelDataProvider.hpp"
#include "common/LapData.hpp"
#include "common/TrackData.hpp"

namespace Rapid::Common::Qt
{

/**
 * The @ref LapDataProvider provides the data for the @ref GenericTableModel.
 * The columns are created dynamically depended on the passed track.
 */
class LapDataProvider : public TableModelDataProvider<LapData>
{
public:
    /**
     * Creates an instance of @ref LapDataProvider
     * @param track The track for creating the column names.
     * @param laps The lap data for the model that shall be shown.
     */
    LapDataProvider(TrackData const& track, std::vector<LapData> laps = {});

private:
    QStringList createColumnNames(TrackData const& track) const noexcept;
};

} // namespace Rapid::Common::Qt

#endif // LAPDATAPROVIDER_HPP
