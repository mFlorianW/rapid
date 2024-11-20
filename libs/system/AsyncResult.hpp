// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "SystemTypes.hpp"
#include <expected>
#include <kdbindings/signal.h>

namespace Rapid::System
{

/**
 * An AsyncResult is a result object for asynchronous operations. The done signal is used as indication that the
 * operation is finshed. The Result must be inherited for the conrect use case. The result must be set with
 * the protected function: @AsyncResult::setResult().
 */
class AsyncResult
{
public:
    /**
     * Creates an Instance of AsyncResult
     */
    AsyncResult();

    /**
     * Destructor
     */
    virtual ~AsyncResult();

    /**
     * Deleted copy constructor
     */
    AsyncResult(AsyncResult const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    AsyncResult& operator=(AsyncResult const& other) = delete;

    /**
     * Move constructor
     */
    AsyncResult(AsyncResult&& ohter) noexcept;

    /**
     * Move assignemnt operator
     */
    AsyncResult& operator=(AsyncResult&& other) noexcept;

    /**
     * Gives the result. The result is not valid (@Result::NotFinshed) as long as the or the
     * @AsyncResult::finished is emitted.
     * @return The asynchronous result
     */
    Result getResult() const noexcept;

    /**
     * Gives the error message of the result. If no error message exists then the
     * string is empty.
     * @return The error message.
     */
    std::string_view getErrorMessage() const noexcept;

    /**
     * Blocks until the async result is there and suspends the current running thread.
     * The function periodically calls the SignalDispatcher for the thread.
     */
    void waitForFinished() noexcept;

    /**
     * The done signal is emitted when the async operation is finished.
     * @param The signal contains a pointer to Async instance for directly requesting the
     * result in the slot.
     */
    KDBindings::Signal<AsyncResult*> done;

    /**
     * Set the result of the AsyncResult and emits the @AsyncResult::finished signal
     * @result The result state of the AsyncResult
     * @errorMessage Sets an optional error message
     */
    void setResult(Result result, std::string const& errorMessage = {}) noexcept;

private:
    Result mResult{Result::NotFinished};
    std::string mErrorMsg{};
};

/**
 * Extends the AsyncResult with a retun value
 */
template <typename T>
class AsyncResultWithValue : public AsyncResult
{
public:
    /**
     * Gives the result of the async operation.
     * If the calls fails the returned value.
     * @return The value or a std::nullopt in case the result is marked as error or not ready.
     */
    std::optional<T> getResultValue() const noexcept
    {
        if (getResult() != Result::Ok) {
            return std::nullopt;
        }
        return mValue;
    }

    /**
     * Sets the result value.
     * This call doesn't emit the done signal so the result can be set as often as needed.
     * The done signal is emitted when the protected @ref AsyncResult::setResult function is called.
     */
    void setResultValue(T const& value)
    {
        mValue = value;
    }

private:
    T mValue;
};

} // namespace Rapid::System
