// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "JsonSerializer.hpp"
#include <utility>

namespace Rapid::Common
{

bool JsonSerializer::serializeTrackData(TrackData const& trackData, JsonObject& jsonObject)
{
    jsonObject["name"] = trackData.getTrackName();

    auto startlineObject = jsonObject.createNestedObject("startline");
    JsonSerializer::serializePositionData(trackData.getStartline(), startlineObject);

    auto finishlineObject = jsonObject.createNestedObject("finishline");
    JsonSerializer::serializePositionData(trackData.getFinishline(), finishlineObject);

    if (trackData.getNumberOfSections() > 0) {
        auto sectorList = jsonObject.createNestedArray("sectors");
        for (std::size_t i = 0; i < trackData.getNumberOfSections(); ++i) {
            auto sectorObject = sectorList.createNestedObject();
            JsonSerializer::serializePositionData(trackData.getSection(i), sectorObject);
        }
    }

    return true;
}

bool JsonSerializer::serializeLapData(LapData const& lapData, JsonObject& jsonObject)
{
    if (lapData.getSectorTimeCount() > 0) {
        auto jsonSectorTimes = jsonObject.createNestedArray("sectors");
        for (std::size_t i = 0; i < lapData.getSectorTimeCount(); ++i) {
            jsonSectorTimes.add(lapData.getSectorTime(i).value_or(Timestamp{}).asString());
        }
    }

    if (lapData.getPositions().size() > 0) {
        auto jsonLogPoints = jsonObject.createNestedArray("log_points");
        for (auto const& gpsPos : std::as_const(lapData.getPositions())) {
            auto pointObj = jsonLogPoints.createNestedObject();
            pointObj["velocity"] = gpsPos.getVelocity().getVelocity();
            pointObj["longitude"] = gpsPos.getPosition().getLongitude();
            pointObj["latitude"] = gpsPos.getPosition().getLatitude();
            pointObj["time"] = gpsPos.getTime().asString();
            pointObj["date"] = gpsPos.getDate().asString();
        }
    }

    return true;
}

bool JsonSerializer::serializePositionData(PositionData const& posData, JsonObject& jsonObject)
{
    jsonObject["latitude"] = std::to_string(posData.getLatitude());
    jsonObject["longitude"] = std::to_string(posData.getLongitude());

    return true;
}

bool JsonSerializer::serializeSessionData(SessionData const& sessionData, JsonObject& jsonObject)
{
    jsonObject["date"] = sessionData.getSessionDate().asString();
    jsonObject["time"] = sessionData.getSessionTime().asString();
    auto trackObject = jsonObject.createNestedObject("track");
    JsonSerializer::serializeTrackData(sessionData.getTrack(), trackObject);
    if (sessionData.getNumberOfLaps() > 0) {
        auto lapArray = jsonObject.createNestedArray("laps");
        for (auto const& lap : std::as_const(sessionData.getLaps())) {
            auto lapObject = lapArray.createNestedObject();
            serializeLapData(lap, lapObject);
        }
    }

    return true;
}

} // namespace Rapid::Common
