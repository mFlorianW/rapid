// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ITrackDetection.hpp"
#include <cstdint>

namespace Rapid::Algorithm
{

class TrackDetection : public ITrackDetection
{
public:
    /**
     * Constructs a TrackDetection within the given radius.
     * @param detectRadiusInMeter The radius that shall be used during the detection.
     */
    TrackDetection(std::uint16_t detectRadiusInMeter);

    /**
     * @copydoc ITrackDataDetection::isOnTrack()
     */
    bool isOnTrack(Common::TrackData const& track, Common::PositionData const& position) const override;

private:
    std::uint16_t mDetectionRadius;
};

} // namespace Rapid::Algorithm
