// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ActiveSessionEndpoint.hpp"
#include <common/JsonSerializer.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::Rest;
using namespace Rapid::Common;

namespace Rapid::Workflow
{

ActiveSessionEndpoint::ActiveSessionEndpoint(IActiveSessionWorkflow* activeSessionWorkflow)
    : Rest::IRestRequestHandler()
    , mActiveSessionWorkflow{activeSessionWorkflow}
{
}

ActiveSessionEndpoint::~ActiveSessionEndpoint() = default;

void ActiveSessionEndpoint::handleRestRequest(Rest::RestRequest& request) noexcept
{
    try {
        if (not isValidPath(request.getPath())) {
            finished.emit(RequestHandleResult::Error, request);
        } else if (request.getPath().getEntry(1) == "track") {
            handleTrackRequest(request);
        } else if (request.getPath().getEntry(1) == "lap") {
            handleLapRequest(request);
        }
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to emit the finished signal. Error: Already emitting");
    }
}

void ActiveSessionEndpoint::handleTrackRequest(Rest::RestRequest& request)
{
    auto result = RequestHandleResult::Ok;
    auto maybeTrack = mActiveSessionWorkflow->getTrack();
    if (maybeTrack.has_value()) {
        auto jsonTrack = JsonSerializer::Track::serialize(maybeTrack.value());
        request.setReturnBody(jsonTrack);
        request.setReturnType(RequestReturnType::Json);
    }
    finished.emit(result, request);
}

void ActiveSessionEndpoint::handleLapRequest(Rest::RestRequest& request)
{
    auto json = nlohmann::ordered_json{};
    json["lapCount"] = mActiveSessionWorkflow->lapCount.get();
    json["currentLap"] = mActiveSessionWorkflow->currentLaptime.get().asString();
    json["currentSector"] = mActiveSessionWorkflow->currentSectorTime.get().asString();
    json["lastLap"] = mActiveSessionWorkflow->lastLaptime.get().asString();
    json["lastSector"] = mActiveSessionWorkflow->lastSectorTime.get().asString();

    request.setReturnBody(json.dump());
    request.setReturnType(RequestReturnType::Json);
    finished.emit(RequestHandleResult::Ok, request);
}

bool ActiveSessionEndpoint::isValidPath(Rest::Path const& path) const noexcept
{
    if (path != "/activeSession/lap" and path != "/activeSession/track") {
        return false;
    }
    return true;
}

} // namespace Rapid::Workflow
