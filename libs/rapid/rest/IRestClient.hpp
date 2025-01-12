// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "RestCall.hpp"
#include "RestRequest.hpp"
#include <string>

namespace Rapid::Rest
{

/**
 * Interface for rest clients then can set send rest request to a server.
 */
class IRestClient
{
public:
    /**
     * Default destructor
     */
    virtual ~IRestClient() noexcept = default;

    /**
     * Deleted copy constructor
     */
    IRestClient(IRestClient const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    IRestClient& operator=(IRestClient const& other) = delete;

    /**
     * Deleted move operator
     */
    IRestClient(IRestClient&& other) = delete;

    /**
     * Deleted move assignment operator
     */
    IRestClient& operator=(IRestClient&& other) = delete;

    /**
     * Set the server address.
     * @param address The address of the server.
     */
    virtual void setServerAddress(std::string const& address) noexcept = 0;

    /**
     * Set the port of the server.
     * @param port The port of the server.
     */
    virtual void setServerPort(std::uint16_t port) noexcept = 0;

    /**
     * Executes a rest request to the server.
     * @request The rest request that shall be executed against the server.
     * @return A RestCall to check for the result of the call.
     */
    virtual std::shared_ptr<RestCall> execute(RestRequest const& request) noexcept = 0;

protected:
    /**
     * Default destructor
     */
    IRestClient() noexcept = default;
};

} // namespace Rapid::Rest
