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
}

void RestServer::registerPostHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    mRestServerImpl->registerPostHandler(root, handler);
}

void RestServer::registerGetHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    mRestServerImpl->registerGetHandler(root, handler);
}

} // namespace Rapid::Rest
