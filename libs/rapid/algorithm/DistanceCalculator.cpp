// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DistanceCalculator.hpp"
#include <cmath>

namespace Rapid::Algorithm::DistanceCalculator
{

float calculateDistance(Common::PositionData const& pos1, Common::PositionData const& pos2)
{
    float lat = (pos1.getLatitude() + pos2.getLatitude()) / 2 * 0.01745f;
    auto dx = static_cast<float>(111300 * cos(lat) * (pos1.getLongitude() - pos2.getLongitude()));
    float dy = 111300 * (pos1.getLatitude() - pos2.getLatitude());
    auto distance = static_cast<float>(sqrt(dx * dx + dy * dy));

    return distance;
}

}; // namespace Rapid::Algorithm::DistanceCalculator
