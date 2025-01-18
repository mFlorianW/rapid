// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "RestRequest.hpp"
#include <kdbindings/signal.h>

namespace Rapid::Rest
{

enum class RequestHandleResult
{
    Ok = 200,
    Created = 201,
    NoContent = 204,
    Error = 500
};

class IRestRequestHandler
{
public:
    /**
     * Defalut destructor
     */
    virtual ~IRestRequestHandler() = default;

    /**
     * Deleted copy constructor.
     */
    IRestRequestHandler(IRestRequestHandler const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    IRestRequestHandler& operator=(IRestRequestHandler const& other) = delete;

    /**
     * Deleted move constructor
     */
    IRestRequestHandler(IRestRequestHandler&& other) = delete;

    /**
     * Deleted move assignment operator
     */
    IRestRequestHandler& operator=(IRestRequestHandler const&& other) = delete;

    /**
     * Function that is called when the handler is registered in the RestServer.
     * The request handler shall also set the return body of the request when needed otherwise the return body is empty.
     * @param request The Request that the service receives and that shall be handled.
     */
    virtual void handleRestRequest(RestRequest& request) noexcept = 0;

    /**
     * This signal is emitted by an @ref IRestRequestHandler when the rest request processing is finished.
     * @param RequestHandleResult The state of the rest request processing.
     * @param RestRequest The REST request itselfs with the optional updated request body for the response.
     */
    KDBindings::Signal<RequestHandleResult, RestRequest> finished;

protected:
    IRestRequestHandler() = default;
};

} // namespace Rapid::Rest
