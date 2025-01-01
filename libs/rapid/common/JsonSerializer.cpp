// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "JsonSerializer.hpp"
#include <charconv>
#include <nlohmann/json.hpp>

namespace Rapid::Common::JsonSerializer
{

std::string serializePosition(float value)
{
    auto buffer = std::array<char, 32>{};
    auto [ptr, _] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
    return std::string{buffer.data(), ptr};
}

namespace Lap
{

nlohmann::ordered_json serialize(LapData const& lap)
{
    auto json = nlohmann::ordered_json{};
    auto sectors = std::vector<nlohmann::ordered_json>{};
    sectors.reserve(lap.getSectorTimeCount());
    for (auto const& sector : lap.getSectorTimes()) {
        sectors.emplace_back(sector.asString());
    }
    json["sectors"] = sectors;

    auto logPoints = std::vector<nlohmann::ordered_json>{};
    logPoints.reserve(lap.getPositions().size());
    for (auto const& gpsPos : lap.getPositions()) {
        auto pointObj = nlohmann::ordered_json{};
        pointObj["velocity"] = gpsPos.getVelocity().getVelocity();
        pointObj["longitude"] = gpsPos.getPosition().getLongitude();
        pointObj["latitude"] = gpsPos.getPosition().getLatitude();
        pointObj["time"] = gpsPos.getTime().asString();
        pointObj["date"] = gpsPos.getDate().asString();
        logPoints.push_back(pointObj);
    }
    json["log_points"] = logPoints;

    return json;
}

} // namespace Lap

namespace Position
{

nlohmann::ordered_json serialize(PositionData const& position)
{
    auto json = nlohmann::ordered_json{};
    json["latitude"] = serializePosition(position.getLatitude());
    json["longitude"] = serializePosition((position.getLongitude()));
    return json;
}

} // namespace Position

namespace Track
{

nlohmann::ordered_json serialize(TrackData const& track)
{
    auto json = nlohmann::ordered_json{};
    json["name"] = track.getTrackName();
    json["startline"] = Position::serialize(track.getStartline());
    json["finishline"] = Position::serialize(track.getFinishline());

    auto jsonSections = std::vector<nlohmann::ordered_json>{};
    jsonSections.reserve(track.getNumberOfSections());
    for (auto const& sectorPos : track.getSections()) {
        jsonSections.emplace_back(Position::serialize(sectorPos));
    }
    json["sectors"] = jsonSections;

    return json;
}

} // namespace Track

namespace Session
{

nlohmann::ordered_json serializeSessionMetaData(SessionMetaData const& session)
{
    auto json = nlohmann::ordered_json{};
    json["id"] = session.getId();
    json["date"] = session.getSessionDate().asString();
    json["time"] = session.getSessionTime().asString();
    json["track"] = Track::serialize(session.getTrack());

    return json;
}

std::string serialize(SessionMetaData const& sessionMetaData)
{
    return serializeSessionMetaData(sessionMetaData).dump();
}

std::string serialize(SessionData const& session)
{
    auto json = serializeSessionMetaData(session);
    auto laps = std::vector<nlohmann::ordered_json>{};
    laps.reserve(session.getNumberOfLaps());
    for (auto const& lap : session.getLaps()) {
        laps.emplace_back(Lap::serialize(lap));
    }
    json["laps"] = laps;

    return json.dump();
}

} // namespace Session

} // namespace Rapid::Common::JsonSerializer
