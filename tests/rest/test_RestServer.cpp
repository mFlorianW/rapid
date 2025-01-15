// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rest/RestServer.hpp"
#include <boost/beast.hpp>
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <testhelper/CompareHelper.hpp>

using namespace Rapid::Rest;
namespace Http = boost::beast::http;
namespace Asio = boost::asio;
namespace Ip = boost::asio::ip;
namespace Beast = boost::beast;

constexpr auto SERVER_ADRESS = "127.0.0.1";

namespace
{

class Request
{
public:
    Request(std::string host, std::string port)
        : mHost{std::move(host)}
        , mPort{std::move(port)}
    {
    }

    ~Request()
    {
        disconnect();
    }

    Request(Request const&) = delete;
    Request& operator=(Request const&) = delete;
    Request(Request&&) = delete;
    Request& operator=(Request&&) = delete;

    bool connect()
    {
        auto const results = mResolver.resolve(mHost, mPort);
        try {
            mTcpStream.connect(results);
        } catch (boost::system::system_error const& e) {
            return false;
        }
        return true;
    }

    bool disconnect()
    {
        mTcpStream.close();
        return true;
    }

    void setVerb(Http::verb verb)
    {
        mVerb = verb;
    }

    void send()
    {
        auto request = Http::request<Http::string_body>{mVerb, "/", 11};
        request.set(Http::field::host, SERVER_ADRESS);
        request.set(Http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        Http::write(mTcpStream, request);
    }

    std::optional<Http::response<Http::string_body>> read()
    {
        if (mResponseRead) {
            return mResponse;
        }

        auto resp = Http::response<Http::string_body>{};
        try {
            auto bytes = Http::read(mTcpStream, mReceiveBuffer, resp);
            if (bytes == 0) {
                return std::nullopt;
            }
            mResponseRead = true;
            mResponse = resp;
            return resp;
        } catch (boost::system::system_error const& error) {
            spdlog::error("Failed to read connection. Error: {} {}", error.what(), error.code().value());
            return std::nullopt;
        }
    }

private:
    std::string mHost;
    std::string mPort;

    std::unique_ptr<Asio::io_context> mIoContext = std::make_unique<Asio::io_context>();
    Ip::tcp::resolver mResolver = Ip::tcp::resolver{*mIoContext};
    Beast::tcp_stream mTcpStream = Beast::tcp_stream{*mIoContext};
    Beast::flat_buffer mReceiveBuffer;
    Http::verb mVerb = Http::verb::get;
    bool mResponseRead = false;
    std::optional<Http::response<Http::string_body>> mResponse;
};

class TestRequestHandler : public IRestRequestHandler
{
public:
    void handleRestRequest(RestRequest& request) noexcept override
    {
        try {
            if (request.getType() == RequestType::Get) {
                mHandlerCalled = true;
                request.setReturnBody(mBody);
                request.setReturnType(mReturnType);
                finished.emit(RequestHandleResult::Ok, request);
            } else if (request.getType() == RequestType::Delete) {
                mDeleteHandlerCalled = true;
                finished.emit(RequestHandleResult::Ok, request);
            }
        } catch (std::exception const& e) {
            SPDLOG_ERROR("Failed to emit finished signal already emitting. Error: {}", e.what());
        }
    }

    bool isHandlerCalled() const noexcept
    {
        return mHandlerCalled;
    }

    bool isDeleteHandlerCalled() const noexcept
    {
        return mDeleteHandlerCalled;
    }

    void setReturnBody(std::string const& body) noexcept
    {
        mBody = body;
    }

    void setRequestReturnType(RequestReturnType returnType) noexcept
    {
        mReturnType = returnType;
    }

private:
    bool mHandlerCalled = false;
    bool mDeleteHandlerCalled = false;
    std::string mBody;
    RequestReturnType mReturnType = RequestReturnType::Txt;
};

} // namespace

constexpr auto timeout = std::chrono::milliseconds{100};

SCENARIO("The running RestServer shall be connectable.")
{
    GIVEN("A running RestServer")
    {
        auto restserver = RestServer{};
        auto result = restserver.start();
        REQUIRE(result == ServerStartResult::Ok);
        WHEN("A connection to the is requested.")
        {
            auto request = Request{"localhost", "27018"};
            auto connected = request.connect();
            THEN("The connection shall be successful.")
            {
                REQUIRE(connected);
            }
        }
    }
}

TEST_CASE("The running RestServer shall send response.")
{
    auto restServer = RestServer{};
    auto result = restServer.start();
    REQUIRE(result == ServerStartResult::Ok);

    auto request = Request{"localhost", "27018"};
    request.connect();
    request.send();

    REQUIRE_COMPARE_WITH_TIMEOUT(request.read().has_value(), true, timeout);
}

SCENARIO("The running RestServer shall handle multiple requests.")
{
    GIVEN("A running RestServer")
    {
        auto restServer = RestServer{};
        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
        WHEN("Multiple request is send.")
        {
            auto request1 = Request{"localhost", "27018"};
            request1.connect();
            request1.send();

            auto request2 = Request{"localhost", "27018"};
            request2.connect();
            request2.send();

            THEN("All requests shall receive a response.")
            {
                REQUIRE_COMPARE_WITH_TIMEOUT(request1.read().has_value(), true, timeout);
                REQUIRE_COMPARE_WITH_TIMEOUT(request2.read().has_value(), true, timeout);
            }
        }
    }
}

SCENARIO("The running server shall forward HTTP GET request to the suitable request handler.")
{
    GIVEN("A running RestServer")
    {
        auto handler = TestRequestHandler{};
        auto restServer = RestServer{};
        restServer.registerGetHandler("/test", &handler);
        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
        WHEN("The GET HTTP request is sent to the server")
        {
            auto request = Request{"localhost", "27018"};
            request.setVerb(Http::verb::get);
            request.connect();
            request.send();

            THEN("The RequestHandler shall be called.")
            {
                REQUIRE_COMPARE_WITH_TIMEOUT(handler.isHandlerCalled(), true, timeout);
            }
        }
    }
}

SCENARIO("The running server shall send the HTTP TXT response body for a valid request.")
{
    GIVEN("A running RestServer")
    {
        auto const expBody = std::string{"Hello World!"};
        auto handler = TestRequestHandler{};
        handler.setReturnBody(expBody);
        auto restServer = RestServer{};
        restServer.registerGetHandler("/test", &handler);
        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
        WHEN("The GET HTTP request is sent to the server")
        {
            auto request = Request{"localhost", "27018"};
            request.setVerb(Http::verb::get);
            request.connect();
            request.send();

            THEN("The RequestHandler shall be called.")
            {
                REQUIRE_COMPARE_WITH_TIMEOUT(request.read().has_value(), true, timeout);
                // NOLINTBEGIN(bugprone-unchecked-optional-access)
                REQUIRE(request.read().value()[Http::field::content_type] == "text/plain");
                REQUIRE(request.read().value().body() == expBody);
                // NOLINTEND(bugprone-unchecked-optional-access)
            }
        }
    }
}

SCENARIO("The running server shall send the HTTP JSON response body for a valid request.")
{
    GIVEN("A running RestServer")
    {
        auto const expBody = std::string{"{}"};
        auto handler = TestRequestHandler{};
        handler.setReturnBody(expBody);
        handler.setRequestReturnType(RequestReturnType::Json);
        auto restServer = RestServer{};
        restServer.registerGetHandler("/test", &handler);
        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
        WHEN("The GET HTTP request is sent to the server")
        {
            auto request = Request{"localhost", "27018"};
            request.setVerb(Http::verb::get);
            request.connect();
            request.send();

            THEN("The RequestHandler shall be called.")
            {
                REQUIRE_COMPARE_WITH_TIMEOUT(request.read().has_value(), true, timeout);
                // NOLINTBEGIN(bugprone-unchecked-optional-access)
                REQUIRE(request.read().value()[Http::field::content_type] == "application/json");
                REQUIRE(request.read().value().body() == expBody);
                // NOLINTEND(bugprone-unchecked-optional-access)
            }
        }
    }
}

TEST_CASE("The RestServer shall handle DELETE requests", "[REST_SERVER]")
{
    auto handler = TestRequestHandler{};
    auto restServer = RestServer{};
    SECTION("The DELETE request is forwarded the registered handler")
    {
        restServer.registerDeleteHandler("/test", &handler);

        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);

        auto request = Request{"localhost", "27018"};
        request.setVerb(Http::verb::delete_);
        request.connect();
        request.send();
        REQUIRE_COMPARE_WITH_TIMEOUT(handler.isDeleteHandlerCalled(), true, timeout);
    }
}
