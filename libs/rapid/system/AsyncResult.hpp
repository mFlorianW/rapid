// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "SystemTypes.hpp"
#include <expected>
#include <kdbindings/signal.h>
#include <thread>

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
     * Disable Move constructor
     */
    AsyncResult(AsyncResult&& ohter) noexcept = delete;

    /**
     * Disable assignemnt operator
     */
    AsyncResult& operator=(AsyncResult&& other) noexcept = delete;

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
     *
     * @note
     * This function should only be called from the threads that owns the @ref AsyncResult.
     * The owning thread is the thread in which the @ref AsyncResult was created.
     */
    void waitForFinished() noexcept;

    /**
     * The done signal is emitted when the async operation is finished.
     * @param The signal contains a pointer to Async instance for directly requesting the
     * result in the slot.
     *
     * @note
     * Use a deferredConnect when the @ref AsyncResult::setResult is called from another thread.
     * The deffreedConnect makes sure that the slot is called in the thread in which the connect happened.
     */
    KDBindings::Signal<AsyncResult*> done;

    /**
     * Set the result of the AsyncResult and emits the @AsyncResult::finished signal.
     *
     * @note
     * The @ref Async::setResult can be called from any thread.
     * Attention when calling @ref AsyncResult::setResult from a different thread use a deferredConnect for the done signal.
     *
     * @result The result state of the AsyncResult
     * @errorMessage Sets an optional error message
     */
    void setResult(Result result, std::string const& errorMessage = {}) noexcept;

protected:
    std::mutex mutable mMutex;

private:
    Result mResult{Result::NotFinished};
    std::string mErrorMsg;
    std::thread::id mThreadId = std::this_thread::get_id();
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
        std::lock_guard<std::mutex> guard{mMutex};
        return mValue;
    }

    /**
     * Sets the result value.
     * This call doesn't emit the done signal so the result can be set as often as needed.
     * The done signal is emitted when the protected @ref AsyncResult::setResult function is called.
     */
    void setResultValue(T const& value)
    {
        std::lock_guard<std::mutex> guard{mMutex};
        mValue = value;
    }

private:
    T mValue;
};

} // namespace Rapid::System
