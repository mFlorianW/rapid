// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Path.hpp"
#include "common/SharedDataPointer.hpp"
#include <string_view>

namespace Rapid::Rest
{

enum class RequestType
{
    Get,
    Post,
    Delete,
};

enum class RequestReturnType
{
    Txt,
    Json
};

struct SharedRestRequest;
class RestRequest
{
public:
    /**
     * Creates an emtpy RestRequest instance
     */
    RestRequest() noexcept;

    /**
     * Creates an instance of the RestRequest.
     * @param requestPath The path of the request
     * @param requestBody The rawdata of the request
     */
    RestRequest(RequestType type, std::string requestPath, std::string requestBody = "") noexcept;

    /**
     * Default destructor.
     */
    ~RestRequest() noexcept;

    /**
     * Copy constructor for RestRequest.
     * @param other The object to copy from.
     */
    RestRequest(RestRequest const& other) noexcept;

    /**
     * Copy assignment operator for RestRequest.
     * @param other The object to copy from.
     * @return RestRequest& A reference to the copied instance.
     */
    RestRequest& operator=(RestRequest const& other) noexcept;

    /**
     * The move constructor for RestRequest.
     * @param other The object to move from.
     */
    RestRequest(RestRequest&& other) noexcept;

    /**
     * The move assignment operator for RestRequest.
     * @param other The object to move from.
     * @return RestRequest& A reference of the moved instance.
     */
    RestRequest& operator=(RestRequest&& other) noexcept;

    /**
     * Gives the type of the request.
     * @return The type of the request.
     */
    RequestType getType() const noexcept;

    /**
     * Gives the body of the request for further processing.
     * @return The raw data.
     */
    [[nodiscard]] std::string_view getRequestBody() const noexcept;

    /**
     * Get the path of the Request. Every call should have a path
     * at least one element the path to the endpoint.
     * @reutrn The path
     */
    [[nodiscard]] Path getPath() const noexcept;

    /**
     * Gives the return body for the request that shall be send back to the caller.
     * The body can be empty when the call doesn't need one.
     * @return The return body for the caller.
     */
    std::string_view getReturnBody() const noexcept;

    /**
     * Sets the return body for the call. This setter is typically called by the
     * request handler.
     * @param returnBody The return body for the caller.
     */
    void setReturnBody(std::string const& returnBody) noexcept;

    /**
     * Gives the return type of the return body.
     * This can be used to set the correct mime type in the HTTP response.
     * The default value is simple text.
     * @return RequestReturnType The format of the request return body.
     */
    RequestReturnType getReturnType() const noexcept;

    /**
     * Sets the type of the of the return body.
     * The @ref RequestReturnType can be used to set the mime in the HTTP response.
     * @param returnType The @ref RequestReturnType of the format of the return body.
     */
    void setReturnType(RequestReturnType const& returnType) noexcept;

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(RestRequest const& lhs, RestRequest const& rhs) noexcept;

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(RestRequest const& lhs, RestRequest const& rhs) noexcept;

private:
    Common::SharedDataPointer<SharedRestRequest> mData;
};

} // namespace Rapid::Rest
