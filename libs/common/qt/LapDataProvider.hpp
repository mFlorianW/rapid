// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LAPDATAPROVIDER_HPP
#define LAPDATAPROVIDER_HPP

#include "LapData.hpp"
#include "TableModelDataProvider.hpp"
#include "TrackData.hpp"

namespace Rapid::Common::Qt
{

class LapDataProvider : public TableModelDataProvider<LapData>
{
public:
    LapDataProvider(TrackData const& track, std::vector<LapData> laps = {});

private:
    QStringList createColumnNames(TrackData const& track) const noexcept;
};

} // namespace Rapid::Common::Qt

#endif // LAPDATAPROVIDER_HPP
