// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IRestServer.hpp"
#include <memory>

namespace Rapid::Rest
{

namespace Private
{
class RestServerImpl;
}

/**
 * This is a facade for the platform dependent implementation of the RestServer
 */
class RestServer : public IRestServer
{
public:
    RestServer();
    /**
     * Default destructor
     */
    ~RestServer() override;

    /**
     * Deleted copy constructor.
     */
    RestServer(RestServer const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    RestServer& operator=(RestServer const& other) = delete;

    /**
     * Deleted move constructor
     */
    RestServer(RestServer&& other) = delete;

    /**
     * Deleted move assignment operator
     */
    RestServer& operator=(RestServer const&& other) = delete;

    /**
     * @copydoc IRestServer::start
     */
    [[nodiscard]] ServerStartResult start() noexcept override;

    /**
     * @copydoc IRestServer::stop
     */
    void stop() noexcept override;

    /**
     * @copydoc IRestServer::registerPostHandler
     */
    void registerPostHandler(std::string const& root, IRestRequestHandler* handler) noexcept override;

    /**
     * copydoc IRestServer::registerGetHandler
     */
    void registerGetHandler(std::string const& root, IRestRequestHandler* handler) noexcept override;

    /**
     * copydoc IRestServer::registerDeleteHandler
     */
    void registerDeleteHandler(std::string const& root, IRestRequestHandler* handler) noexcept override;

private:
    std::unique_ptr<Private::RestServerImpl> mRestServerImpl;
};

} // namespace Rapid::Rest
