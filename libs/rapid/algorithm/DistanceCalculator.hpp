// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <common/PositionData.hpp>

namespace Rapid::Algorithm::DistanceCalculator
{
/**
 * Calculates the between tow positions. The position is only accurate
 * when the expected distance is in the range of 1-2km.
 * @param pos1 The first position.
 * @param pos2 The second position.
 * @return float  The distance between the to points.
 */
float calculateDistance(Common::PositionData const& pos1, Common::PositionData const& pos2);
}; // namespace Rapid::Algorithm::DistanceCalculator
