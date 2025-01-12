// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LAPTIMER_HPP
#define LAPTIMER_HPP

#include "algorithm/ILaptimer.hpp"

namespace Rapid::TestHelper
{

class Laptimer final : public Algorithm::ILaptimer
{
public:
    Laptimer();

    void setTrack(Common::TrackData const& track) override;

    void updatePositionAndTime(Common::GpsPositionData const& data) override;

    Common::Timestamp getLastLaptime() const override;

    Common::Timestamp getLastSectorTime() const override;

    std::vector<Common::Timestamp> sectorTimes;

    Common::GpsPositionData lastPostionDateTime;
};

} // namespace Rapid::TestHelper

#endif // LAPTIMER_HPP
