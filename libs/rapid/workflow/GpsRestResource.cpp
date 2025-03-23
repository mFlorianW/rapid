// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsRestResource.hpp"
#include <common/JsonSerializer.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::Rest;

namespace Rapid::Workflow
{
namespace
{

std::string convertFixModeToString(Positioning::GpsFixMode fixMode) noexcept
{
    auto strFixMode = std::string{"nofix"};
    switch (fixMode) {
    case Positioning::GpsFixMode::Fix2d:
        strFixMode = "fix2d";
        break;
    case Positioning::GpsFixMode::Fix3d:
        strFixMode = "fix3d";
        break;
    default:
        //do nothing
        break;
    }
    return strFixMode;
}

bool isValidPath(Path const& path)
{
    auto entry = path.getEntry(1);
    if (entry.has_value() and (entry == "fixmode" or entry == "satellites" or entry == "position")) {
        return true;
    }
    return false;
}

} // namespace

GpsRestResource::GpsRestResource(Positioning::IGpsInformationProvider* gpsInfoProvider,
                                 Positioning::IGpsPositionProvider* gpsPosProvider)
    : mGpsInfoProvider{gpsInfoProvider}
    , mGpsPosProvider{gpsPosProvider}
{
}

void GpsRestResource::handleRestRequest(Rest::RestRequest& request) noexcept
{
    try {
        if (not isValidPath(request.getPath())) {
            finished.emit(RequestHandleResult::Error, request);
            return;
        }
        auto jsonRsp = nlohmann::ordered_json{};
        if (request.getPath().getEntry(1).value_or("") == "fixmode") {
            jsonRsp["fixmode"] = convertFixModeToString(mGpsInfoProvider->getGpsFixMode());
            request.setReturnType(RequestReturnType::Json);
            request.setReturnBody(jsonRsp.dump());
        } else if (request.getPath().getEntry(1).value_or("") == "satellites") {
            jsonRsp["satellites"] = mGpsInfoProvider->getNumbersOfStatelite();
            request.setReturnBody(jsonRsp.dump());
        } else if (request.getPath().getEntry(1).value_or("") == "position") {
            request.setReturnBody(Common::JsonSerializer::Position::serialize(mGpsPosProvider->gpsPosition.get()));
        }
        request.setReturnType(RequestReturnType::Json);
        finished.emit(RequestHandleResult::Ok, request);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to emit the finished signal. Error: {}", e.what());
    }
}

} // namespace Rapid::Workflow
