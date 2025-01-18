// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_REST_PRIVATE_CLIENTCONNECTION_HPP
#define RAPID_REST_PRIVATE_CLIENTCONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <rest/IRestRequestHandler.hpp>
#include <rest/RestRequest.hpp>

namespace Rapid::Rest::Private
{

class ClientConnection
{
public:
    ClientConnection(boost::asio::io_context& ioService);

    ~ClientConnection();

    ClientConnection(ClientConnection const&) = delete;
    ClientConnection& operator=(ClientConnection const&) = delete;
    ClientConnection(ClientConnection&&) = delete;
    ClientConnection& operator=(ClientConnection&&) = delete;

    boost::asio::ip::tcp::socket& getSocket() noexcept;

    void handleConnection();

    RestRequest getRestRequest() const noexcept;

    void sendResponse(RequestHandleResult result, std::string const& body, std::string const& bodyType);

    KDBindings::Signal<RestRequest, ClientConnection*> requestReceived;
    KDBindings::Signal<ClientConnection*> finished;

private:
    boost::asio::ip::tcp::socket mSocket;
    boost::beast::flat_buffer mReceiveBuffer;
    boost::beast::http::request<boost::beast::http::string_body> mRequest;
    boost::beast::http::response<boost::beast::http::string_body> mResponse;
    RestRequest mRestRequest;
};

}; // namespace Rapid::Rest::Private

#endif // !RAPID_REST_PRIVATE_CLIENTCONNECTION_HPP
