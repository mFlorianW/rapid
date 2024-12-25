// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "IRestClient.hpp"
#include "RestRequest.hpp"

namespace Rapid::TestHelper
{
class RestSessionDownloaderClient : public Rest::IRestClient
{
    void setServerAddress(std::string const& address) noexcept override;

    void setServerPort(std::uint16_t port) noexcept override;

    std::shared_ptr<Rest::RestCall> execute(Rest::RestRequest const& request) noexcept override;

    Rest::RestRequest mLastRequest;
};
} // namespace Rapid::TestHelper
