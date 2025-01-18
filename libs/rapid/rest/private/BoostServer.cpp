// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "BoostServer.hpp"
#include "RestServerImpl.hpp"

namespace Asio = boost::asio;
namespace Ip = boost::asio::ip;
namespace Beast = boost::beast;
namespace Http = boost::beast::http;

namespace Rapid::Rest::Private
{

BoostServer::BoostServer(RestServerImpl* server)
    : mServer{server}
{
    auto const address = Ip::make_address("0.0.0.0");
    auto const port = std::uint16_t{27018};
    mContext = std::make_unique<boost::asio::io_context>();
    mAcceptor = std::make_unique<Ip::tcp::acceptor>(*mContext, Ip::tcp::endpoint{address, port});
    mTcpSocket = std::make_unique<Ip::tcp::socket>(*mContext);
}

BoostServer::~BoostServer() = default;

void BoostServer::start()
{
    // create first client connection and for a connection
    accept();
}
void BoostServer::accept()
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

void BoostServer::onIncomingConnection(Beast::error_code errorCode, ClientConnection* connection)
{
    if (mClientConnections.count(connection) > 0) {
        std::ignore = connection->requestReceived.connect([this](auto&& request, auto&& conn) {
            if (mClientConnections.count(conn) > 0) {
                {
                    std::lock_guard<std::mutex> guard{mMutex};
                    mPendingRequests.insert({conn, conn->getRestRequest()});
                }
                System::EventLoop::postEvent(mServer,
                                             std::make_unique<System::Event>(System::Event::Type::HttpRequestReceived));
            }
        });
        mClientConnections[connection]->handleConnection();
    }
}

bool BoostServer::hasPendingRequests() const noexcept
{
    return not mPendingRequests.empty();
}

Request BoostServer::getNextPendingRequest() noexcept
{
    assert(not mPendingRequests.empty());
    std::lock_guard<std::mutex> guard{mMutex};
    auto request = Request{mPendingRequests.begin()->first, mPendingRequests.begin()->second};
    mPendingRequests.erase(mPendingRequests.begin());
    return request;
}

} // namespace Rapid::Rest::Private
