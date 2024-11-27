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

    auto startlineObject = jsonObject["startline"].to<JsonObject>();
    JsonSerializer::serializePositionData(trackData.getStartline(), startlineObject);

    auto finishlineObject = jsonObject["finishline"].to<JsonObject>();
    JsonSerializer::serializePositionData(trackData.getFinishline(), finishlineObject);

    if (trackData.getNumberOfSections() > 0) {
        auto sectorList = jsonObject["sectors"].to<JsonArray>();
        for (std::size_t i = 0; i < trackData.getNumberOfSections(); ++i) {
            auto sectorObject = sectorList.add<JsonObject>();
            JsonSerializer::serializePositionData(trackData.getSection(i), sectorObject);
        }
    }
    return true;
}

bool JsonSerializer::serializeLapData(LapData const& lapData, JsonObject& jsonObject)
{
    if (lapData.getSectorTimeCount() > 0) {
        auto jsonSectorTimes = jsonObject["sectors"].to<JsonArray>();
        for (std::size_t i = 0; i < lapData.getSectorTimeCount(); ++i) {
            jsonSectorTimes.add(lapData.getSectorTime(i).value_or(Timestamp{}).asString());
        }
    }

    if (lapData.getPositions().size() > 0) {
        auto jsonLogPoints = jsonObject["log_points"].to<JsonArray>();
        for (auto const& gpsPos : std::as_const(lapData.getPositions())) {
            auto pointObj = jsonLogPoints.add<JsonObject>();
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
    auto trackObject = jsonObject["track"].to<JsonObject>();
    JsonSerializer::serializeTrackData(sessionData.getTrack(), trackObject);
    if (sessionData.getNumberOfLaps() > 0) {
        auto lapArray = jsonObject["laps"].to<JsonArray>();
        for (auto const& lap : std::as_const(sessionData.getLaps())) {
            auto lapObject = lapArray.add<JsonObject>();
            serializeLapData(lap, lapObject);
        }
    }
    return true;
}

} // namespace Rapid::Common
