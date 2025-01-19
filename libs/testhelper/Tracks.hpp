// SPDX-FileCopyrightText: 2024 - 2025 All contributors
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
std::string getTrackAsJson();
} // namespace Rapid::TestHelper::Tracks

#endif //! TRACKS_HPP
