// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "TrackDetection.hpp"
#include "DistanceCalculator.hpp"
#include <cmath>

using namespace Rapid::Common;

namespace Rapid::Algorithm
{

TrackDetection::TrackDetection(std::uint16_t detectRadiusInMeter)
    : mDetectionRadius(detectRadiusInMeter)
{
}

bool TrackDetection::isOnTrack(TrackData const& track, PositionData const& position) const
{
    auto distance = static_cast<std::uint16_t>(DistanceCalculator::calculateDistance(track.getFinishline(), position));
    return distance <= mDetectionRadius;
}

} // namespace Rapid::Algorithm
