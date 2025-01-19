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

/**
 * @brief Inteface definition for a REST server implementation.
 *
 * @details The REST server manages incoming HTTP Requests and dispatch the there requests on registered handler.
 *          The handler can be registered by it's root path and every request for that root is dispatched to the handler.
 *          There a different types of handler for the REST Types, GET, POST, PUT and DELETE.
 */
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
    IRestServer(IRestServer&& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    IRestServer& operator=(IRestServer const& other) = delete;
    IRestServer& operator=(IRestServer const&& other) = delete;

    /**
     * Deleted move constructor
     */

    /**
     * Deleted move assignment operator
     */

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
     * @brief Registers a POST method handler in the server.
     *
     * @details The handler is called when a request for the registered root is received.
     *          The Server can only have on handler for each route at the time.
     *          The last set handler for a route is called by the server.
     *
     * @note The Server doesn't take any ownership about the handler object.
     *       The caller must guarantee that object lives as long as the server.
     *
     * @param root The root route for the request handler.
     * @param handler The handler object that is called when the type receives a request.
     */
    virtual void registerPostHandler(std::string const& root, IRestRequestHandler* handler) noexcept = 0;

    /**
     * @brief Registers a GET method handler in the server.
     *
     * @details The handler is called when a request for the registered root is received.
     *          The Server can only have on handler for each route at the time.
     *          The last set handler for a route is called by the server.
     *
     * @note The Server doesn't take any ownership about the handler object.
     *       The caller must guarantee that object lives as long as the server.
     *
     * @param type The root route for the request handler.
     * @param handler The handler object that is called when the type receives a request.
     */
    virtual void registerGetHandler(std::string const& root, IRestRequestHandler* handler) noexcept = 0;

    /**
     * @brief Registers a DELETE handler in the server which is called on request of the passed root dir.
     *
     * @details The handler is called when a request for the registered root is received.
     *          The Server can only have one handler for each route at the time.
     *          The last set handler for a route is called by the server.
     *
     * @note The Server doesn't take any ownership about the handler object.
     *       The caller must guarantee that object lives as long as the server.
     *
     * @param type The root route for the to register the handler.
     * @param handler The handler object that is called when the type receives a request.
     */
    virtual void registerDeleteHandler(std::string const& root, IRestRequestHandler* handler) noexcept = 0;

    /**
     * @brief Registers a PUT handler in the server which is called on request of the passed root dir.
     *
     * @details The handler is called when a request for the registered root is received.
     *          The Server can only have one handler for each route at the time.
     *          The last set handler for a route is called by the server.
     *
     * @note The Server doesn't take any ownership about the handler object.
     *       The caller must guarantee that object lives as long as the server.
     *
     * @param type The root route for the to register the handler.
     * @param handler The handler object that is called when the type receives a request.
     */
    virtual void registerPutHandler(std::string const& root, IRestRequestHandler* handler) noexcept = 0;

protected:
    IRestServer() = default;
};
} // namespace Rapid::Rest
