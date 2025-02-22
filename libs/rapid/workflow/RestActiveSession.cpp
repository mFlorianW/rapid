// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestActiveSession.hpp"
#include <cassert>
#include <common/JsonDeserializer.hpp>
#include <nlohmann/json.hpp>
#include <rest/IRestClient.hpp>
#include <rest/RestCall.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::Rest;
using namespace Rapid::Common;

namespace Rapid::Workflow
{

RestActiveSession::RestActiveSession(Rest::IRestClient* restClient)
    : mRestClient{restClient}
{
    assert(restClient != nullptr);
}

void RestActiveSession::updateTrackData() noexcept
{
    auto constexpr trackUrl = "/activeSession/track";
    mPendingTrackCall = mRestClient->execute(RestRequest{RequestType::Get, trackUrl});
    mPendingTrackCallConnection = mPendingTrackCall->finished.connect([this](auto* call) {
        onDownloadTrackFinished(call);
    });
}

void RestActiveSession::updateLapData() noexcept
{
    auto constexpr lapUrl = "/activeSession/lap";
    mPendingLapCall = mRestClient->execute(RestRequest{RequestType::Get, lapUrl});
    mPendingLapCallConnection = mPendingLapCall->finished.connect([this](auto* call) {
        onUpdateLapInformationFinished(call);
    });
}

void RestActiveSession::onDownloadTrackFinished(RestCall* call) noexcept
{
    if (call->getResult() == Rest::RestCallResult::Success) {
        try {
            auto trackData = Common::JsonDeserializer::Track::derserialize(call->getData());
            if (trackData.has_value()) {
                track.set(trackData.value());
            }
        } catch (std::exception const& e) {
            SPDLOG_ERROR("Failed to parse track download. Error: {}", e.what());
        }
    } else {
        SPDLOG_ERROR("Track download failed with error. Error: {}", call->getData());
    }
    mPendingTrackCallConnection->disconnect();
}

void RestActiveSession::onUpdateLapInformationFinished(Rest::RestCall* call) noexcept
{
    auto jsonResult = nlohmann::ordered_json{};
    constexpr auto lapCountId = "lapCount";
    constexpr auto currentLapId = "currentLap";
    constexpr auto currentSectorId = "currentSector";
    constexpr auto lastLapId = "lastLap";
    constexpr auto lastSectorId = "lastSector";
    try {
        auto lapData = jsonResult.parse(call->getData());
        if (lapData.count(lapCountId) >= 0 and lapData[lapCountId] != lapCount.get()) {
            lapCount.set(lapData[lapCountId]);
        }
        if (lapData.count(currentLapId) >= 0 and Timestamp{lapData[currentLapId]} != currentLap.get()) {
            currentLap.set(Timestamp{lapData[currentLapId]});
        }
        if (lapData.count(currentSectorId) >= 0 and Timestamp{lapData[currentSectorId]} != currentSector.get()) {
            currentSector.set(Timestamp{lapData[currentSectorId]});
        }
        if (lapData.count(lastLapId) >= 0 and Timestamp{lapData[lastLapId]} != lastSector.get()) {
            lastLap.set(Timestamp{lapData[lastLapId]});
        }
        if (lapData.count(lastSectorId) >= 0 and Timestamp{lapData[lastSectorId]} != lastSector.get()) {
            lastSector.set(Timestamp{lapData[lastSectorId]});
        }
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to parse JSON lap request. Error: {}", e.what());
    }
    mPendingLapCallConnection->disconnect();
}

} // namespace Rapid::Workflow
