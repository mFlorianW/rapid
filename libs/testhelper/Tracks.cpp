// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Tracks.hpp"
#include "Positions.hpp"
#include "common/PositionData.hpp"

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

Common::TrackData getTrack()
{
    auto pos = PositionData{52.0, 11.2};
    auto track = TrackData{};
    track.setTrackName("Track");
    track.setStartline(pos);
    track.setFinishline(pos);
    track.setSections({pos, pos});

    return track;
}

std::string getTrackAsJson()
{
    // clang-format off
    constexpr auto json =
        "{"
            "\"name\":\"Track\","
            "\"startline\":{"
                "\"latitude\":\"52\","
                "\"longitude\":\"11.2\""
            "},"
            "\"finishline\":{"
                "\"latitude\":\"52\","
                "\"longitude\":\"11.2\""
            "},"
            "\"sectors\":["
                "{"
                   "\"latitude\":\"52\","
                   "\"longitude\":\"11.2\""
                "},"
                "{"
                   "\"latitude\":\"52\","
                   "\"longitude\":\"11.2\""
                "}"
            "]"
        "}";
    // clang-format on
    return json;
}

} // namespace Rapid::TestHelper::Tracks
