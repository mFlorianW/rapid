// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include "JsonSerializer.hpp"
#include "Positions.hpp"
#include "Sessions.hpp"
#include "Tracks.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;
using namespace Rapid::TestHelper;

TEST_CASE("JSONSerializer shall serialize PositionData to JSON.")
{
    constexpr auto expectedJsonObject = R"({"latitude":"52.025833","longitude":"11.279166"})";
    ArduinoJson::StaticJsonDocument<1024> jsonDoc;
    auto jsonRoot = jsonDoc.to<JsonObject>();

    JsonSerializer::serializePositionData(Positions::getOscherslebenPositionCamp(), jsonRoot);
    std::string result;
    ArduinoJson::serializeJson(jsonDoc, result);

    REQUIRE(result == expectedJsonObject);
}

TEST_CASE("JSONSerializer shall serialize the TrackData without sector to JSON")
{
    // clang-format off
    constexpr auto expectedJson = "{"
        "\"name\":\"TrackWithoutSector\","
        "\"startline\":{"
            "\"latitude\":\"52.025833\","
            "\"longitude\":\"11.279166\""
        "},"
        "\"finishline\":{"
            "\"latitude\":\"52.025833\","
            "\"longitude\":\"11.279166\""
        "}"
    "}";
    // clang-format on

    ArduinoJson::StaticJsonDocument<1024> jsonDoc;
    auto jsonRoot = jsonDoc.to<JsonObject>();

    JsonSerializer::serializeTrackData(Tracks::getTrackWithoutSector(), jsonRoot);
    std::string result;
    ArduinoJson::serializeJson(jsonDoc, result);

    REQUIRE(result == expectedJson);
}

TEST_CASE("JSONSerializer shall serialize the TrackData to JSON")
{
    // clang-format off
    constexpr auto expectedJson = "{"
        "\"name\":\"Oschersleben\","
        "\"startline\":{"
            "\"latitude\":\"52.025833\","
            "\"longitude\":\"11.279166\""
        "},"
        "\"finishline\":{"
            "\"latitude\":\"52.025833\","
            "\"longitude\":\"11.279166\""
        "},"
        "\"sectors\":["
        "{"
            "\"latitude\":\"52.025833\","
            "\"longitude\":\"11.279166\""
        "},"
        "{"
            "\"latitude\":\"52.025833\","
            "\"longitude\":\"11.279166\""
        "}"
        "]"
    "}";
    // clang-format on
    ArduinoJson::StaticJsonDocument<1024> jsonDoc;
    auto jsonRoot = jsonDoc.to<JsonObject>();

    JsonSerializer::serializeTrackData(Tracks::getOscherslebenTrack(), jsonRoot);
    std::string result;
    ArduinoJson::serializeJson(jsonDoc, result);

    REQUIRE(result == expectedJson);
}

TEST_CASE("JSONSerializer shall serialize the LapData to JSON")
{
    // clang-format off
    constexpr auto expectedResult =
    "{"
    "\"sectors\":["
        "\"00:00:25.134\","
        "\"00:00:25.144\""
        "]"
    "}";
    // clang-format on
    Timestamp sectorTime1;
    sectorTime1.setHour(0);
    sectorTime1.setMinute(0);
    sectorTime1.setSecond(25);
    sectorTime1.setFractionalOfSecond(134);
    Timestamp sectorTime2;
    sectorTime2.setHour(0);
    sectorTime2.setMinute(0);
    sectorTime2.setSecond(25);
    sectorTime2.setFractionalOfSecond(144);
    LapData lap;
    lap.addSectorTime(sectorTime1);
    lap.addSectorTime(sectorTime2);
    ArduinoJson::StaticJsonDocument<1024> jsonDoc;
    auto jsonRoot = jsonDoc.to<JsonObject>();

    JsonSerializer::serializeLapData(lap, jsonRoot);
    std::string result;
    ArduinoJson::serializeJson(jsonDoc, result);

    REQUIRE(result == expectedResult);
}

TEST_CASE("JSONSerializer shall serialize SessionData to JSON")
{
    auto session = Sessions::getTestSession();
    ArduinoJson::StaticJsonDocument<8192> jsonDoc;
    auto jsonRoot = jsonDoc.to<JsonObject>();

    JsonSerializer::serializeSessionData(session, jsonRoot);
    std::string result;
    ArduinoJson::serializeJson(jsonDoc, result);

    REQUIRE(result == Sessions::getTestSessionAsJson());
}
