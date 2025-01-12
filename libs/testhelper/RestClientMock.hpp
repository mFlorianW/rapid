// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TESTHELPER_RESTCLIENTMOCK_HPP
#define TESTHELPER_RESTCLIENTMOCK_HPP

#include <rest/IRestClient.hpp>
#include <spdlog/spdlog.h>
#include <trompeloeil.hpp>

namespace Rapid::TestHelper
{

class RestClientMock : public Rest::IRestClient
{
public:
    MAKE_MOCK(setServerAddress, auto(std::string const&)->void, noexcept override);
    MAKE_MOCK(setServerPort, auto(std::uint16_t)->void, noexcept override);
    MAKE_MOCK(execute, auto(Rest::RestRequest const&)->std::shared_ptr<Rest::RestCall>, noexcept override);
};

class RestCallMock : public Rest::RestCall
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

} // namespace Rapid::TestHelper

#endif // TESTHELPER_RESTCLIENTMOCK_HPP
