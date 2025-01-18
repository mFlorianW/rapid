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
        request.setReturnType(mReturnType);
        request.setReturnBody(mBody);
        try {
            if (request.getType() == RequestType::Get) {
                mHandlerCalled = true;
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

class TestFixture
{
public:
    TestRequestHandler handler;
    RestServer restServer;
    Request request{"localhost", "27018"};
    std::string const expBody{"{}"};

    TestFixture()
    {
        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
    }
};

} // namespace

constexpr auto timeout = std::chrono::milliseconds{100};

TEST_CASE_METHOD(TestFixture, "The running RestServer shall be connectable.", "[REST_SERVER_BASIC]")
{
    auto request = Request{"localhost", "27018"};
    auto connected = request.connect();
    REQUIRE(connected);
}

TEST_CASE_METHOD(TestFixture, "The running RestServer shall send response.", "[REST_SERVER_BASIC]")
{
    auto request = Request{"localhost", "27018"};
    request.connect();
    request.send();

    REQUIRE_COMPARE_WITH_TIMEOUT(request.read().has_value(), true, timeout);
}

TEST_CASE_METHOD(TestFixture, "The running RestServer shall handle multiple requests.", "[REST_SERVER_BASIC]")
{
    auto request1 = Request{"localhost", "27018"};
    request1.connect();
    request1.send();

    auto request2 = Request{"localhost", "27018"};
    request2.connect();
    request2.send();

    REQUIRE_COMPARE_WITH_TIMEOUT(request1.read().has_value(), true, timeout);
    REQUIRE_COMPARE_WITH_TIMEOUT(request2.read().has_value(), true, timeout);
}

TEST_CASE_METHOD(TestFixture, "The RestServer shall be start and stopable multiple times", "[REST_SERVER_STOP]")
{
    SECTION("Call start multiple times")
    {
        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
        result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
    }

    SECTION("Call start and stop multiple times")
    {
        auto result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
        restServer.stop();
        result = restServer.start();
        REQUIRE(result == ServerStartResult::Ok);
        restServer.stop();
    }
}

TEST_CASE_METHOD(TestFixture, "The running server shall handle HTTP GET request.", "[REST_SERVER_GET]")
{
    restServer.registerGetHandler("/test", std::addressof(handler));

    SECTION("The GET request is forwarded the registered handler")
    {
        request.setVerb(Http::verb::get);
        request.connect();
        request.send();
        REQUIRE_COMPARE_WITH_TIMEOUT(handler.isHandlerCalled(), true, timeout);
    }

    SECTION("The GET request shall return with 200 when body exists")
    {
        handler.setReturnBody(expBody);
        request.setVerb(Http::verb::get);
        request.connect();
        request.send();
        REQUIRE_COMPARE_WITH_TIMEOUT(request.read().has_value(), true, timeout);
        auto response = request.read().value_or(Http::response<Http::string_body>{});
        CHECK(response.result() == Http::status::ok);
        CHECK(response[Http::field::content_type] == "text/plain");
        REQUIRE(response.body() == expBody);
    }
}

TEST_CASE_METHOD(TestFixture, "The RestServer shall handle DELETE requests", "[REST_SERVER_DELETE]")
{
    restServer.registerDeleteHandler("/test", &handler);

    SECTION("The DELETE request is forwarded the registered handler")
    {
        request.setVerb(Http::verb::delete_);
        request.connect();
        request.send();
        REQUIRE_COMPARE_WITH_TIMEOUT(handler.isDeleteHandlerCalled(), true, timeout);
    }

    SECTION("The DELETE shall return with 204 without response body")
    {
        request.setVerb(Http::verb::delete_);
        request.connect();
        request.send();
        REQUIRE_COMPARE_WITH_TIMEOUT(request.read().has_value(), true, timeout);
        auto response = request.read().value_or(Http::response<Http::string_body>{});
        REQUIRE(response.result() == Http::status::no_content);
    }

    SECTION("The DELETE shall return with 200 with response body")
    {
        handler.setReturnBody(expBody);
        request.setVerb(Http::verb::delete_);
        request.connect();
        request.send();
        REQUIRE_COMPARE_WITH_TIMEOUT(request.read().has_value(), true, timeout);
        auto response = request.read().value_or(Http::response<Http::string_body>{});
        REQUIRE(response.result() == Http::status::ok);
        REQUIRE(response.body() == expBody);
    }
}
