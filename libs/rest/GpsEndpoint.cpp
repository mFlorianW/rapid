// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsEndpoint.hpp"
#include "RestRequest.hpp"
#include <ArduinoJson.hpp>

using namespace Rapid::Common;

namespace Rapid::Rest
{
GpsEndpoint::GpsEndpoint() = default;
GpsEndpoint::~GpsEndpoint() = default;

RequestHandleResult GpsEndpoint::handleRestRequest(RestRequest& request) noexcept
{
    try {

        auto jsonDoc = ArduinoJson::DynamicJsonDocument{512};
        if ((ArduinoJson::deserializeJson(jsonDoc, request.getRequestBody()) !=
             ArduinoJson::DeserializationError::Ok) &&
            jsonDoc.containsKey("date") && jsonDoc.containsKey("time") && jsonDoc.containsKey("latitude") &&
            jsonDoc.containsKey("longitude")) {
            return RequestHandleResult::Error;
        }
        auto newPos = GpsPositionData{};
        newPos.setDate({jsonDoc["date"].as<std::string>()});
        newPos.setTime({jsonDoc["time"].as<std::string>()});
        newPos.setPosition(
            {std::stof(jsonDoc["latitude"].as<std::string>()), std::stof(jsonDoc["longitude"].as<std::string>())});

        gpsPosition = newPos;

    } catch (std::exception const& e) {
        std::cerr << "Failed to handle REST request unexpected error during deserialization. Error:" << e.what()
                  << "\n";
        return RequestHandleResult::Error;
    }

    return RequestHandleResult::Ok;
}
} // namespace Rapid::Rest
