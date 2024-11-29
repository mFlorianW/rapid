// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "JsonDeserializer.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace Rapid::Common::JsonDeserializer::Session
{

namespace
{

std::optional<PositionData> parsePosition(nlohmann::ordered_json const& jsonPosition)
{
    if (not jsonPosition.contains("latitude")) {
        SPDLOG_CRITICAL("Failed to deserialize position. Error: Missing required parameter latitude missing");
        return std::nullopt;
    }
    if (not jsonPosition.contains("longitude")) {
        SPDLOG_CRITICAL("Failed to deserialize position. Error: Missing required parameter longitude missing");
        return std::nullopt;
    }
    std::string positionLatitude = jsonPosition["latitude"];
    std::string positionLongitude = jsonPosition["longitude"];
    PositionData position{std::stof(positionLatitude), std::stof(positionLongitude)};
    return position;
}

std::optional<TrackData> parseTrack(nlohmann::ordered_json const& jsonTrack)
{
    if (not jsonTrack.contains("name")) {
        SPDLOG_CRITICAL("Failed to deserialize track. Error: Missing required parameter name missing");
        return std::nullopt;
    }
    if (not jsonTrack.contains("finishline")) {
        SPDLOG_CRITICAL("Failed to deserialize track. Error: Missing required parameter finishline missing");
        return std::nullopt;
    }
    TrackData trackData;
    auto trackName = jsonTrack["name"];
    trackData.setTrackName(trackName);

    try {
        if (jsonTrack.contains("startline")) {
            auto startline = parsePosition(jsonTrack["startline"]);
            if (startline.has_value()) {
                trackData.setStartline(startline.value());
            }
        }

        auto finishLine = parsePosition(jsonTrack["finishline"]);
        if (not finishLine) {
            return std::nullopt;
        }
        trackData.setFinishline(finishLine.value());

        auto jsonSectors = jsonTrack["sectors"];
        auto sections = std::vector<PositionData>{};
        sections.reserve(jsonSectors.size());
        for (auto const& sector : jsonSectors) {
            auto pos = parsePosition(sector);
            if (pos.has_value()) {
                sections.push_back(pos.value());
            } else {
                sections.clear();
                break;
            }
        }
        if (sections.size() > 0) {
            trackData.setSections(sections);
        }

        return trackData;
    } catch (std::invalid_argument& e) {
        SPDLOG_CRITICAL("Failed deserilize lap data invalid argument.{}", e.what());
        return std::nullopt;
    } catch (std::out_of_range& e) {
        SPDLOG_CRITICAL("Failed deserilize lap data invalid argument.{}", e.what());
        return std::nullopt;
    }
    return std::nullopt;
}

std::vector<LapData> parseLaps(nlohmann::ordered_json const& jsonLaps)
{
    auto laps = std::vector<LapData>();
    laps.reserve(jsonLaps.size());
    for (auto const& jsonLap : jsonLaps) {
        auto lap = LapData{};
        for (auto const& sector : jsonLap["sectors"]) {
            lap.addSectorTime(Timestamp{sector});
        }

        auto const jsonLogPoints = jsonLap["log_points"];
        auto logPoints = std::vector<GpsPositionData>{};
        try {
            for (auto const& jsonLogPoint : jsonLogPoints) {
                auto const gpsPosition =
                    GpsPositionData{PositionData{jsonLogPoint["latitude"], jsonLogPoint["longitude"]},
                                    Timestamp{jsonLogPoint["time"]},
                                    Date{jsonLogPoint["date"]},
                                    VelocityData{jsonLogPoint["velocity"]}};
                logPoints.push_back(gpsPosition);
            }
        } catch (std::invalid_argument& e) {
            SPDLOG_CRITICAL("Failed deserilize lap data invalid argument.{}", e.what());
            return {};
        } catch (std::out_of_range& e) {
            SPDLOG_CRITICAL("Failed deserilize lap data invalid argument.{}", e.what());
            return {};
        }
        lap.overwritePositions(logPoints);
        laps.push_back(lap);
    }
    return laps;
}

} // namespace

std::optional<SessionData> deserialize(std::string const& rawData)
{
    auto json = nlohmann::ordered_json{};
    try {
        auto jsonSession = json.parse(rawData);
        auto sessionDate = Date(jsonSession["date"]);
        auto sessionTime = Timestamp(jsonSession["time"]);
        auto track = parseTrack(jsonSession["track"]);
        auto laps = parseLaps(jsonSession["laps"]);
        auto session = SessionData{track.value_or(TrackData{}), sessionDate, sessionTime};
        session.addLaps(laps);
        return session;
    } catch (nlohmann::json::exception const& e) {
        SPDLOG_CRITICAL("Failed to deserialize session. {}", e.what());
        return std::nullopt;
    }
}

} // namespace Rapid::Common::JsonDeserializer::Session
