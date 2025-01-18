// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ClientConnection.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::Rest::Private
{

namespace
{

RequestType getRequestType(boost::beast::http::request<boost::beast::http::string_body> const& request)
{
    switch (request.method()) {
    case boost::beast::http::verb::get:
        return RequestType::Get;
    case boost::beast::http::verb::delete_:
        return RequestType::Delete;
    case boost::beast::http::verb::post:
        return RequestType::Post;
    case boost::beast::http::verb::put:
        return RequestType::Put;
    default:
        return RequestType::Get;
    }
}

} // namespace

ClientConnection::ClientConnection(boost::asio::io_context& ioService)
    : mSocket{ioService}
{
}

ClientConnection::~ClientConnection()
{
    try {
        mSocket.close();
    } catch (boost::system::system_error& e) {
        spdlog::error("Socket closing throws an error. Error:", e.what());
    }
}

boost::asio::ip::tcp::socket& ClientConnection::getSocket() noexcept
{
    return mSocket;
}

void ClientConnection::handleConnection()
{
    boost::beast::http::async_read(
        mSocket,
        mReceiveBuffer,
        mRequest,
        [this](boost::beast::error_code const& ec, std::size_t bytes) {
            mRestRequest = RestRequest{getRequestType(mRequest), std::string{mRequest.target()}, mRequest.body()};
            requestReceived.emit(mRestRequest, this);
        });
}

RestRequest ClientConnection::getRestRequest() const noexcept
{
    return mRestRequest;
}

void ClientConnection::sendResponse(RequestHandleResult result, std::string const& body, std::string const& bodyType)
{
    mResponse.set(boost::beast::http::field::host, "lappy.org");
    mResponse.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    mResponse.keep_alive(false);
    mResponse.prepare_payload();

    if (not body.empty()) {
        mResponse.body() = body;
        mResponse.set(boost::beast::http::field::content_type, bodyType);
        mResponse.content_length(mResponse.body().size());
    }
    mResponse.result(static_cast<std::uint32_t>(result));
    boost::beast::http::async_write(mSocket, mResponse, [this](boost::beast::error_code errorCode, std::size_t) {
        if (errorCode) {
            SPDLOG_ERROR("Failed to send response. Error code: {}", errorCode.value());
        }
        finished.emit(this);
    });
}

} // namespace Rapid::Rest::Private
