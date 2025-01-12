// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <common/PositionData.hpp>
#include <common/TrackData.hpp>

namespace Rapid::Algorithm
{

class ITrackDetection
{
public:
    /**
     * Default virtual destructor
     */
    virtual ~ITrackDetection() = default;

    /**
     * Default copy operator
     */
    ITrackDetection(ITrackDetection const&) = default;

    /**
     * Default copy operator
     */
    ITrackDetection& operator=(ITrackDetection const&) = default;

    /**
     * Default move operator
     */
    ITrackDetection(ITrackDetection&&) noexcept = default;

    /**
     * Default move operator
     */
    ITrackDetection& operator=(ITrackDetection&&) noexcept = default;

    /**
     * Checks if the given position is on the given track.
     * @param track The track to test if the position is on that track.
     * @param position The given position that shall be used for the check.
     * @return true The position is on the track.
     * @return false The position is not on the track.
     */
    virtual bool isOnTrack(Common::TrackData const& track, Common::PositionData const& position) const = 0;

protected:
    /**
     * Default constructor
     */
    ITrackDetection() = default;
};

} // namespace Rapid::Algorithm
