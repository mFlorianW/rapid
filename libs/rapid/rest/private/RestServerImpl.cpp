// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestServerImpl.hpp"
#include "kdbindings/signal.h"
#include <boost/asio.hpp>
#include <memory>
#include <rest/private/BoostServer.hpp>
#include <rest/private/ClientConnection.hpp>
#include <spdlog/spdlog.h>
#include <system/EventLoop.hpp>

namespace Asio = boost::asio;
namespace Ip = boost::asio::ip;
namespace Beast = boost::beast;
namespace Http = boost::beast::http;

namespace Rapid::Rest::Private
{

namespace
{

std::string getReturnType(RestRequest const& request)
{
    if (request.getReturnType() == RequestReturnType::Json) {
        return std::string{"application/json"};
    }
    return std::string{"text/plain"};
}

} // namespace

RestServerImpl::RestServerImpl() = default;

RestServerImpl::~RestServerImpl() noexcept
{
    stop();
}

ServerStartResult RestServerImpl::start() noexcept
{
    if (mRunning) {
        return ServerStartResult::Ok;
    }
    mServerThread = std::thread([this]() {
        auto server = BoostServer{this};
        mBoostServer = &server;
        // Stop was called during startup.
        // To avoid a deadlock don't call start at all.
        if (not mRunning) {
            auto guard = std::lock_guard(mMutex);
            mBoostServer = nullptr;
            return;
        }
        mBoostServer->start();
        auto guard = std::lock_guard(mMutex);
        mBoostServer = nullptr;
        mRunning = false;
    });
    mRunning = true;
    return ServerStartResult::Ok;
}

void RestServerImpl::stop() noexcept
{
    mRunning = false;
    {
        auto guard = std::lock_guard(mMutex);
        // It can happen that stop is called and start function is not completed yet.
        // Then the boost server is not set and is still nullptr.
        if (mBoostServer != nullptr and mServerThread.joinable()) {
            mBoostServer->stop();
            mBoostServer = nullptr;
        }
    }
    if (mServerThread.joinable()) {
        mServerThread.join();
    }
}

void RestServerImpl::registerPostHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    registerHandler(root, handler, mPostHandlers, mProcessingPostRequests);
}

void RestServerImpl::registerGetHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    registerHandler(root, handler, mGetHandlers, mProcessingGetRequests);
}

void RestServerImpl::registerDeleteHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    registerHandler(root, handler, mDeleteHandlers, mProcessingDeleteRequests);
}

void RestServerImpl::registerPutHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    registerHandler(root, handler, mPutHandlers, mProcessingPutRequests);
}

bool RestServerImpl::handleEvent(System::Event* event) noexcept
{
    if (event->getEventType() == System::Event::Type::HttpRequestReceived) {
        if (mBoostServer->hasPendingRequests()) {
            auto nextRequest = mBoostServer->getNextPendingRequest();
            auto conn = std::get<0>(nextRequest);
            auto request = std::get<1>(nextRequest);
            if (request.getType() == Rest::RequestType::Get) {
                handleRequest(request, conn, mGetHandlers, mProcessingGetRequests);
            } else if (request.getType() == Rest::RequestType::Delete) {
                handleRequest(request, conn, mDeleteHandlers, mProcessingDeleteRequests);
            } else if (request.getType() == Rest::RequestType::Post) {
                handleRequest(request, conn, mPostHandlers, mProcessingPostRequests);
            } else if (request.getType() == RequestType::Put) {
                handleRequest(request, conn, mPutHandlers, mProcessingPutRequests);
            }
        }
        return true;
    }
    return false;
}

void RestServerImpl::registerHandler(std::string const& root,
                                     IRestRequestHandler* handler,
                                     auto& handlerCache,
                                     auto& processCache)
{
    auto entry = std::make_unique<HandlerEntry>();
    entry->handler = handler;
    entry->mFinishedConnection = handler->finished.connect([this, &processCache](auto&& result, auto&& restRequest) {
        this->handleFinishedRequest(result, restRequest, processCache);
    });
    handlerCache.emplace(root, std::move(entry));
}

void RestServerImpl::handleRequest(RestRequest& request,
                                   ClientConnection* connection,
                                   auto& requestCache,
                                   auto& processingCache)
{
    try {
        auto const path = request.getPath().getPath();
        for (auto& [handlerPath, handler] : requestCache) {
            if (path.starts_with(handlerPath)) {
                processingCache.insert({path, connection});
                handler->handler->handleRestRequest(request);
                return;
            }
        }
        connection->sendResponse(RequestHandleResult::Error,
                                 std::string{request.getReturnBody()},
                                 getReturnType(request));
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to handle request for unexpected exception: {}", e.what());
    }
}

void RestServerImpl::handleFinishedRequest(RequestHandleResult& result,
                                           RestRequest const& request,
                                           auto& requestCache) noexcept
{
    try {
        auto conn = requestCache.at(request.getPath().getPath());
        conn->sendResponse(result, std::string{request.getReturnBody()}, getReturnType(request));
        requestCache.erase(request.getPath().getPath());
    } catch (std::out_of_range const& e) {
        SPDLOG_ERROR("No connection found for rquest on \"{}\"", request.getPath().getPath());
    } catch (...) {
        SPDLOG_ERROR("Failed to send response for request for {}", request.getPath().getPath());
    }
}

} // namespace Rapid::Rest::Private
