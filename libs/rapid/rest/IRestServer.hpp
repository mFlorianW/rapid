// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IRestRequestHandler.hpp"

namespace Rapid::Rest
{
enum ServerStartResult
{
    Ok,
    Error
};

enum class PostHandler
{
    PositionHandler,
};

enum class GetHandler
{
    SessionHandler,
};

class IRestServer
{
public:
    /**
     * Default Destructor
     */
    virtual ~IRestServer() = default;

    /**
     * Deleted copy constructor.
     */
    IRestServer(IRestServer const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    IRestServer& operator=(IRestServer const& other) = delete;

    /**
     * Deleted move constructor
     */
    IRestServer(IRestServer&& other) = delete;

    /**
     * Deleted move assignment operator
     */
    IRestServer& operator=(IRestServer const&& other) = delete;

    /**
     * Starts the REST server.
     * @return ServerStartResult::Ok Server successful started.
     * @return ServerStartResult::Error Failed to start the server, something somewhere went wrong.
     */
    [[nodiscard]] virtual ServerStartResult start() noexcept = 0;

    /**
     * Stops the REST server
     */
    virtual void stop() noexcept = 0;

    /**
     * Registers a POST method handler in the server. The handler is called when a request for the registered root is
     * received. The server can only have one handle for one route at the time. The last set handle for the route is
     * called by the server.
     *
     * @note
     * The Server doesn't take any ownership about the handler object. The caller must guarantee that object lives as
     * long as the server.
     *
     * @param root The root route for the request handler.
     * @param handler The handler object that is called when the type receives a request.
     */
    virtual void registerPostHandler(std::string const& root, IRestRequestHandler* handler) noexcept = 0;

    /**
     * Registers a GET method handler in the server. The handler is called when a request for the registered root is
     * received. The Server can only have on handler for each route at the time. The last set handler for a route is
     * called by the server.
     *
     * @note
     * The Server doesn't take any ownership about the handler object. The caller must guarantee that object lives as
     * long as the server.
     *
     * @param type The root route for the to register the handler.
     * @param handler The handler object that is called when the type receives a request.
     */
    virtual void registerGetHandler(std::string const& root, IRestRequestHandler* handler) noexcept = 0;

protected:
    IRestServer() = default;
};
} // namespace Rapid::Rest
