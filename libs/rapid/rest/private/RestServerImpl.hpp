// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "rest/IRestServer.hpp"
#include <boost/beast.hpp>
#include <system/EventHandler.hpp>
#include <thread>
#include <unordered_map>

namespace Rapid::Rest::Private
{
class BoostServer;
class ClientConnection;
class RestServerImpl : public Rest::IRestServer, public System::EventHandler
{
public:
    RestServerImpl();
    /**
     * Default destructor
     */
    ~RestServerImpl() noexcept override;

    /**
     * Deleted copy constructor.
     */
    RestServerImpl(RestServerImpl const& other) = delete;
    RestServerImpl(RestServerImpl&& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    RestServerImpl& operator=(RestServerImpl const& other) = delete;
    RestServerImpl& operator=(RestServerImpl const&& other) = delete;

    /**
     * Deleted move constructor
     */

    /**
     * Deleted move assignment operator
     */

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
     * copydoc IRestServer::registerGetHandler
     */
    void registerDeleteHandler(std::string const& root, IRestRequestHandler* handler) noexcept override;

    /**
     * copydoc IRestServer::registerPostHandler
     */
    void registerPutHandler(std::string const& root, IRestRequestHandler* handler) noexcept override;

    /**
     * @copydoc Rapid::System::EventHandler
     */
    bool handleEvent(System::Event* event) noexcept override;

private:
    void registerHandler(std::string const& root, IRestRequestHandler* handler, auto& handlerCache, auto& processCache);
    void handleRequest(RestRequest& request, ClientConnection* connection, auto& requestCache, auto& processingCache);
    void handleFinishedRequest(RequestHandleResult& result, RestRequest const& request, auto& requestCache) noexcept;

    std::thread mServerThread;
    BoostServer* mBoostServer = nullptr;
    std::atomic<bool> mRunning;
    std::mutex mMutex;

    struct HandlerEntry
    {
        IRestRequestHandler* handler = nullptr;
        KDBindings::ScopedConnection mFinishedConnection;
    };
    using HandlerEntryPtr = std::unique_ptr<HandlerEntry>;

    std::unordered_map<std::string, HandlerEntryPtr> mGetHandlers;
    std::unordered_map<std::string, HandlerEntryPtr> mDeleteHandlers;
    std::unordered_map<std::string, HandlerEntryPtr> mPostHandlers;
    std::unordered_map<std::string, HandlerEntryPtr> mPutHandlers;
    std::unordered_map<std::string_view, ClientConnection*> mProcessingGetRequests;
    std::unordered_map<std::string_view, ClientConnection*> mProcessingDeleteRequests;
    std::unordered_map<std::string_view, ClientConnection*> mProcessingPostRequests;
    std::unordered_map<std::string_view, ClientConnection*> mProcessingPutRequests;
};

} // namespace Rapid::Rest::Private
