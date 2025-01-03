// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TRACKS_HPP
#define TRACKS_HPP

#include "common/TrackData.hpp"

namespace Rapid::TestHelper::Tracks
{
Common::TrackData getTrackWithoutSector();
Common::TrackData getOscherslebenTrack();
Common::TrackData getOscherslebenTrack2();
Common::TrackData getTrack();
} // namespace Rapid::TestHelper::Tracks

#endif //! TRACKS_HPP
