// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsRestResourceReader.hpp"
#include <nlohmann/json.hpp>
#include <rest/RestRequest.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::Rest;
using namespace Rapid::Positioning;

namespace Rapid::Workflow
{

namespace
{
GpsFixMode convertFixMode(std::string const& rawMode)
{
    auto fixMode = GpsFixMode::NoFix;
    if (rawMode == "fix2d") {
        fixMode = GpsFixMode::Fix2d;
    } else if (rawMode == "fix3d") {
        fixMode = GpsFixMode::Fix3d;
    }
    return fixMode;
}
} // namespace

GpsRestResourceReader::GpsRestResourceReader(Rest::IRestClient* restClient)
    : mRestClient{restClient}
{
}

GpsRestResourceReader::~GpsRestResourceReader() = default;

std::shared_ptr<FixModeResult> GpsRestResourceReader::getFixMode() noexcept
{
    auto request = RestRequest{RequestType::Get, "/gps/fixmode"};
    auto requestEntry = RequestEntry<FixModeResult>{};
    auto result = std::make_shared<FixModeResult>();
    auto call = mRestClient->execute(request);
    requestEntry.call = call;
    requestEntry.result = result;
    if (call->isFinished()) {
        mFixModeRequests.insert(std::make_pair(call.get(), std::move(requestEntry)));
        handleFixModeRequestFinished(call.get());
    } else {
        requestEntry.connection = call->finished.connect([this](auto&& call) {
            handleFixModeRequestFinished(call);
        });
        mFixModeRequests.insert(std::make_pair(call.get(), std::move(requestEntry)));
    }
    return result;
}

std::shared_ptr<SatellitesResult> GpsRestResourceReader::getSatellites() noexcept
{
    auto request = RestRequest{RequestType::Get, "/gps/satellites"};
    auto requestEntry = RequestEntry<SatellitesResult>{};
    auto result = std::make_shared<SatellitesResult>();
    auto call = mRestClient->execute(request);
    requestEntry.call = call;
    requestEntry.result = result;
    if (call->isFinished()) {
        mSatellitesRequests.insert({call.get(), std::move(requestEntry)});
        handleSatellitesRequestFinished(call.get());
    } else {
        requestEntry.connection = call->finished.connect([this](auto&& call) {
            handleSatellitesRequestFinished(call);
        });
        mSatellitesRequests.insert({call.get(), std::move(requestEntry)});
    }
    return result;
}

void GpsRestResourceReader::handleFixModeRequestFinished(Rest::RestCall* call)
{
    try {
        auto& request = mFixModeRequests.at(call);
        auto result = System::Result::Error;
        if (call->getResult() == RestCallResult::Success) {
            try {
                auto json = nlohmann::ordered_json{}.parse(call->getData());
                if (json.contains("fixmode")) {
                    request.result->setResultValue(convertFixMode(json["fixmode"]));
                }
                result = System::Result::Ok;
            } catch (nlohmann::json::exception const& e) {
                SPDLOG_ERROR("Failed to parse fix mode response", e.what());
            }
        }
        request.result->setResult(result);
        mFixModeRequests.erase(call);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Fix mode request handler called without request. Error: {}", e.what());
    }
}

void GpsRestResourceReader::handleSatellitesRequestFinished(Rest::RestCall* call)
{
    try {
        auto& request = mSatellitesRequests.at(call);
        auto result = System::Result::Error;
        if (call->getResult() == RestCallResult::Success) {
            try {
                auto json = nlohmann::ordered_json{}.parse(call->getData());
                if (json.contains("satellites")) {
                    request.result->setResultValue(json["satellites"]);
                }
                result = System::Result::Ok;
            } catch (nlohmann::json::exception const& e) {
                SPDLOG_ERROR("Failed to parse fix mode response", e.what());
            }
        }
        request.result->setResult(result);
        mSatellitesRequests.erase(call);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Satellites request handler called without request. Error: {}", e.what());
    }
}

} // namespace Rapid::Workflow
