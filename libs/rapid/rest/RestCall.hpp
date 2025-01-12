// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <kdbindings/signal.h>

namespace Rapid::Rest
{

enum class RestCallResult
{
    Unknown,
    Success,
    Error,
};

/**
 * A RestCall contains the asynchronous REST call to an endpoint.
 */
class RestCall
{
public:
    /**
     * Creates an Rest instance
     */
    RestCall() noexcept = default;

    /**
     * Default destructor
     */
    virtual ~RestCall() noexcept = default;

    /**
     * Disabled copy constructor
     */
    RestCall(RestCall const& other) = delete;

    /**
     * Disabled copy assignment operator
     */
    RestCall& operator=(RestCall const& other) = delete;

    /**
     * Disabled copy assignment operator
     */
    RestCall(RestCall&&) = delete;

    /**
     * Disabled move assignment
     */
    RestCall& operator=(RestCall&& other) = delete;

    /**
     * Gives the status of the call.
     * @return True the call is finished, in all other cases false.
     */
    bool isFinished() const noexcept;

    /**
     * Gives the result of the REST call. The return type is only valid after the finished
     * signal was emitted or the isFinished return true.
     * @return The result of the call.
     */
    RestCallResult getResult() const noexcept;

    /**
     * Gives the raw data of the call. The data is only valid after the call is finished.
     * @return The raw data of the call.
     */
    std::string getData() const noexcept;

    /**
     * Set the call data of the call.
     * @param data The data of the call.
     */
    void setData(std::string const& data) noexcept;

    /**
     * This sinal is emitted when the RestCall finished.
     */
    KDBindings::Signal<RestCall*> finished;

protected:
    /**
     * Set the result of the call. The implementation should emit the finshed as well. Before setting the
     * result the setCallData function should be called to set the call data.
     * Must be implemented by concrect instance which are mainy coupled to the rest client implementation.
     */
    virtual void setCallResult(RestCallResult result) noexcept = 0;

protected:
    RestCallResult mResult{RestCallResult::Unknown};

private:
    std::string mRawData;
};

} // namespace Rapid::Rest
