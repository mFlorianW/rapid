// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestServer.hpp"
#include "private/RestServerImpl.hpp"

namespace Rapid::Rest
{

RestServer::RestServer()
    : mRestServerImpl{std::make_unique<Private::RestServerImpl>()}
{
}

RestServer::~RestServer() = default;

ServerStartResult RestServer::start() noexcept
{
    return mRestServerImpl->start();
}

void RestServer::stop() noexcept
{
    mRestServerImpl->stop();
}

void RestServer::registerPostHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    mRestServerImpl->registerPostHandler(root, handler);
}

void RestServer::registerGetHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    mRestServerImpl->registerGetHandler(root, handler);
}

void RestServer::registerDeleteHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    mRestServerImpl->registerDeleteHandler(root, handler);
}

void RestServer::registerPutHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    mRestServerImpl->registerPutHandler(root, handler);
}

} // namespace Rapid::Rest
