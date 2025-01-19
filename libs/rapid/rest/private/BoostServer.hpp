// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_REST_PRIVATE_BOOSTSERVER_HPP
#define RAPID_REST_PRIVATE_BOOSTSERVER_HPP

#include "ClientConnection.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <rest/RestRequest.hpp>
#include <system/EventLoop.hpp>

namespace Rapid::Rest::Private
{

using Request = std::tuple<Private::ClientConnection*, Rapid::Rest::RestRequest>;

class RestServerImpl;

class BoostServer
{
public:
    BoostServer(RestServerImpl* server);

    ~BoostServer();
    BoostServer(BoostServer const&) = delete;
    BoostServer& operator=(BoostServer const&) = delete;
    BoostServer(BoostServer&&) noexcept = delete;
    BoostServer& operator=(BoostServer&&) noexcept = delete;

    void start();

    void accept();

    void onIncomingConnection(boost::beast::error_code errorCode, ClientConnection* connection);

    void stop()
    {
        mContext->stop();
    }

    bool hasPendingRequests() const noexcept;

    Request getNextPendingRequest() noexcept;

private:
    std::unique_ptr<boost::asio::io_context> mContext;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> mAcceptor;
    std::shared_ptr<boost::asio::ip::tcp::socket> mTcpSocket;
    std::unordered_map<ClientConnection*, std::unique_ptr<ClientConnection>> mClientConnections;
    std::unordered_map<ClientConnection*, RestRequest> mPendingRequests;
    std::mutex mMutex;
    RestServerImpl* mServer;
};

} // namespace Rapid::Rest::Private

#endif // !RAPID_REST_PRIVATE_BOOSTSERVER_HPP
