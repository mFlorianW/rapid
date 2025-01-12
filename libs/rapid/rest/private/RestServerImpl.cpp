// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestServerImpl.hpp"
#include "kdbindings/signal.h"
#include <boost/asio.hpp>
#include <memory>
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

class ClientConnection
{
public:
    ClientConnection(Asio::io_context& ioService)
        : mSocket{ioService}
    {
    }

    ~ClientConnection()
    {
        try {
            mSocket.close();
        } catch (boost::system::system_error& e) {
            spdlog::error("Socket closing throws an error. Error:", e.what());
        }
    }

    ClientConnection(ClientConnection const&) = delete;
    ClientConnection& operator=(ClientConnection const&) = delete;
    ClientConnection(ClientConnection&&) = delete;
    ClientConnection& operator=(ClientConnection&&) = delete;

    Ip::tcp::socket& getSocket()
    {
        return mSocket;
    }

    void handleConnection()
    {
        Http::async_read(mSocket, mReceiveBuffer, mRequest, [this](Beast::error_code const& ec, std::size_t bytes) {
            mRestRequest = RestRequest{RequestType::Get, std::string{mRequest.target()}, mRequest.body()};
            requestReceived.emit(mRestRequest, this);
        });
    }

    RestRequest getRestRequest() const noexcept
    {
        return mRestRequest;
    }

    void sendResponse(RequestHandleResult result, std::string const& body, std::string const& bodyType)
    {
        auto status = result == RequestHandleResult::Ok ? Http::status::ok : Http::status::bad_request;
        mResponse.set(Http::field::host, "lappy.org");
        mResponse.set(Http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        mResponse.result(status);
        mResponse.keep_alive(false);
        mResponse.prepare_payload();

        if (not body.empty()) {
            mResponse.body() = body;
            mResponse.set(Http::field::content_type, bodyType);
            mResponse.content_length(mResponse.body().size());
        }
        Http::async_write(mSocket, mResponse, [this](Beast::error_code errorCode, std::size_t) {
            if (errorCode) {
                spdlog::error("Failed to send response. Error code: {}", errorCode.value());
            }
            finished.emit(this);
        });
    }

    KDBindings::Signal<RestRequest, ClientConnection*> requestReceived;
    KDBindings::Signal<ClientConnection*> finished;

private:
private:
    Ip::tcp::socket mSocket;
    Beast::flat_buffer mReceiveBuffer;
    Http::request<Http::string_body> mRequest;
    Http::response<Http::string_body> mResponse;
    RestRequest mRestRequest;
};

using Request = std::tuple<ClientConnection*, Rapid::Rest::RestRequest>;

class BoostServer
{
public:
    BoostServer(RestServerImpl* server)
        : mServer{server}
    {
        auto const address = Ip::make_address("0.0.0.0");
        auto const port = std::uint16_t{27018};
        mContext = std::make_unique<boost::asio::io_context>();
        mAcceptor = std::make_unique<Ip::tcp::acceptor>(*mContext, Ip::tcp::endpoint{address, port});
        mTcpSocket = std::make_unique<Ip::tcp::socket>(*mContext);
    }

    void start()
    {
        // create first client connection and for a connection
        accept();
    }

    void accept()
    {
        auto connection = std::make_unique<ClientConnection>(*mContext);
        std::ignore = connection->finished.connect([this](auto* client) {
            if (mClientConnections.count(client) > 0) {
                mClientConnections.erase(client);
            }
        });
        mAcceptor->async_accept(connection->getSocket(), [this, capture0 = connection.get()](auto&& errorCode) {
            if (not errorCode) {
                onIncomingConnection(errorCode, capture0);
            }
            accept();
        });
        mClientConnections.insert({connection.get(), std::move(connection)});
        mContext->run();
    }

    void onIncomingConnection(Beast::error_code errorCode, ClientConnection* connection)
    {
        if (mClientConnections.count(connection) > 0) {
            std::ignore = connection->requestReceived.connect([this](auto&& request, auto&& conn) {
                if (mClientConnections.count(conn) > 0) {
                    {
                        std::lock_guard<std::mutex> guard{mMutex};
                        mPendingRequests.insert({conn, conn->getRestRequest()});
                    }
                    System::EventLoop::postEvent(
                        mServer,
                        std::make_unique<System::Event>(System::Event::Type::HttpRequestReceived));
                }
            });
            mClientConnections[connection]->handleConnection();
        }
    }

    void stop()
    {
        mContext->stop();
    }

    bool hasPendingRequests() const noexcept
    {
        return not mPendingRequests.empty();
    }

    Request getNextPendingRequest() noexcept
    {
        assert(not mPendingRequests.empty());
        std::lock_guard<std::mutex> guard{mMutex};
        auto request = Request{mPendingRequests.begin()->first, mPendingRequests.begin()->second};
        mPendingRequests.erase(mPendingRequests.begin());
        return request;
    }

private:
    std::unique_ptr<boost::asio::io_context> mContext;
    std::unique_ptr<Ip::tcp::acceptor> mAcceptor;
    std::shared_ptr<Ip::tcp::socket> mTcpSocket;
    std::unordered_map<ClientConnection*, std::unique_ptr<ClientConnection>> mClientConnections;
    std::unordered_map<ClientConnection*, RestRequest> mPendingRequests;
    std::mutex mMutex;
    RestServerImpl* mServer;
};

RestServerImpl::RestServerImpl() = default;

RestServerImpl::~RestServerImpl()
{
    if (mBoostServer != nullptr && mServerThread.joinable()) {
        mBoostServer->stop();
        mServerThread.join();
    }
}

ServerStartResult RestServerImpl::start() noexcept
{
    mServerThread = std::thread{[this]() {
        auto server = BoostServer{this};
        mBoostServer = &server;
        mBoostServer->start();
    }};
    return ServerStartResult::Ok;
}

void RestServerImpl::stop() noexcept
{
    mBoostServer->stop();
}

void RestServerImpl::registerPostHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
}

void RestServerImpl::registerGetHandler(std::string const& root, IRestRequestHandler* handler) noexcept
{
    mGetHandlers.insert({root, handler});
    std::ignore = handler->finished.connect([this](auto&& result, auto&& restRequest) {
        handleFinishedGetRequest(result, restRequest);
    });
}

bool RestServerImpl::handleEvent(System::Event* event) noexcept
{
    if (event->getEventType() == System::Event::Type::HttpRequestReceived) {
        if (mBoostServer->hasPendingRequests()) {
            auto nextRequest = mBoostServer->getNextPendingRequest();
            auto conn = std::get<0>(nextRequest);
            auto request = std::get<1>(nextRequest);
            if (request.getType() == Rest::RequestType::Get) {
                handleGetRequest(request, conn);
            }
        }
        return true;
    }
    return false;
}

void RestServerImpl::handleGetRequest(RestRequest& request, ClientConnection* conn) noexcept
{
    try {
        auto const path = request.getPath();
        for (auto& [path, handler] : mGetHandlers) {
            if (path.rfind("path")) {
                mProcessingGetRequests.insert({request.getPath().getPath(), conn});
                handler->handleRestRequest(request);
                return;
            }
        }
        conn->sendResponse(RequestHandleResult::Error, std::string{request.getReturnBody()}, getReturnType(request));
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed to handle request for unexpected exception: {}", e.what());
    }
}

void RestServerImpl::handleFinishedGetRequest(RequestHandleResult result, RestRequest const& request)
{
    try {
        auto conn = mProcessingGetRequests.at(request.getPath().getPath());
        conn->sendResponse(result, std::string{request.getReturnBody()}, getReturnType(request));
        mProcessingGetRequests.erase(request.getPath().getPath());
    } catch (std::out_of_range const& e) {
        SPDLOG_ERROR("No connection found for rquest on \"{}\"", request.getPath().getPath());
    } catch (...) {
        SPDLOG_ERROR("Failed to send response for request for {}", request.getPath().getPath());
    }
}

} // namespace Rapid::Rest::Private
