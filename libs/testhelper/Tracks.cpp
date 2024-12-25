// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Tracks.hpp"
#include "PositionData.hpp"
#include "Positions.hpp"

using namespace Rapid::Common;

namespace Rapid::TestHelper::Tracks
{
namespace
{
Common::TrackData createTrackWithoutSector()
{
    auto trackWithoutSector = TrackData{};
    trackWithoutSector.setTrackName("TrackWithoutSector");
    trackWithoutSector.setStartline(Positions::getOscherslebenPositionCamp());
    trackWithoutSector.setFinishline(Positions::getOscherslebenPositionCamp());

    return trackWithoutSector;
}

Common::TrackData createOscherslebenTrack()
{
    auto oschersleben = TrackData{};
    oschersleben.setTrackName("Oschersleben");
    oschersleben.setStartline(Positions::getOscherslebenPositionCamp());
    oschersleben.setFinishline(Positions::getOscherslebenPositionCamp());
    oschersleben.setSections({Positions::getOscherslebenPositionCamp(), Positions::getOscherslebenPositionCamp()});

    return oschersleben;
}

Common::TrackData createOscherslebenTrack2()
{
    auto oschersleben = TrackData{};
    oschersleben.setTrackName("Oschersleben");
    oschersleben.setStartline(Positions::getOscherslebenPositionCamp());
    oschersleben.setFinishline(Positions::getOscherslebenPositionCamp());
    oschersleben.setSections({Positions::getOscherslebenPositionCamp(), Common::PositionData{52.258335, 11.279166666}});

    return oschersleben;
}

} // namespace

TrackData getOscherslebenTrack()
{
    return createOscherslebenTrack();
}

TrackData getOscherslebenTrack2()
{
    return createOscherslebenTrack2();
}

TrackData getTrackWithoutSector()
{
    return createTrackWithoutSector();
}

} // namespace Rapid::TestHelper::Tracks
