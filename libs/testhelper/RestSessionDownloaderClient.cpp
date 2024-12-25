// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionDownloaderClient.hpp"
#include "Sessions.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::TestHelper
{

class TestRestCall : public Rest::RestCall
{
public:
    void setCallResult(Rest::RestCallResult result) noexcept override
    {
        try {
            mResult = result;
            finished.emit(this);
        } catch (std::exception const& e) {
            SPDLOG_ERROR("Failed to emit finished signal. Error: Already emitting");
        }
    }
};

void RestSessionDownloaderClient::setServerAddress(std::string const& address) noexcept
{
}

void RestSessionDownloaderClient::setServerPort(std::uint16_t port) noexcept
{
}

std::shared_ptr<Rest::RestCall> RestSessionDownloaderClient::execute(Rest::RestRequest const& request) noexcept
{
    mLastRequest = request;
    auto call = std::make_shared<TestRestCall>();
    if (request.getPath().getPath() == "/sessions") {
        call->setData({R"({"count":2})"});
    } else if (request.getPath().getPath() == "/sessions/0") {
        call->setData(TestHelper::Sessions::getTestSessionAsJson());
    }
    call->setCallResult(Rest::RestCallResult::Success);
    return call;
}

} // namespace Rapid::TestHelper
