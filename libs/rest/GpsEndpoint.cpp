// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsEndpoint.hpp"
#include "RestRequest.hpp"
#include "nlohmann/json.hpp"
#include <spdlog/spdlog.h>

using namespace Rapid::Common;

namespace Rapid::Rest
{
GpsEndpoint::GpsEndpoint() = default;
GpsEndpoint::~GpsEndpoint() = default;

RequestHandleResult GpsEndpoint::handleRestRequest(RestRequest& request) noexcept
{
    try {
        auto jsonDoc = nlohmann::ordered_json{}.parse(request.getRequestBody());
        auto newPos = GpsPositionData{};
        newPos.setDate({jsonDoc["date"]});
        newPos.setTime({jsonDoc["time"]});
        std::string latitude = jsonDoc["latitude"];
        std::string longitude = jsonDoc["longitude"];
        newPos.setPosition({std::stof(latitude), std::stof(longitude)});
        gpsPosition = newPos;

    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to handle REST request unexpected error during deserialization. Error: {}", e.what());
        return RequestHandleResult::Error;
    }

    return RequestHandleResult::Ok;
}
} // namespace Rapid::Rest
