// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "EventHandler.hpp"
#include "EventLoop.hpp"
#include <future>
#include <iostream>
#include <kdbindings/signal.h>

namespace Rapid::System
{

template <class T>
class FutureWatcher : public EventHandler
{
public:
    /**
     * Creates FutureWatcher instance
     */
    FutureWatcher() noexcept = default;

    /**
     * Creates a FutureWatcher instance and starts observing the future.
     * @param future The future to observe
     */
    FutureWatcher(std::future<T> future) noexcept
    {
        setFuture(std::move(future));
    }

    /**
     * default constructor
     */
    ~FutureWatcher() noexcept override
    {
        if (mFutureObserver.joinable()) {
            mFutureObserver.join();
        }
    }

    /**
     * Deleted copy constructor
     */
    FutureWatcher(FutureWatcher const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    FutureWatcher& operator=(FutureWatcher const& other) = delete;

    /**
     * Deleted move constructor
     */
    FutureWatcher(FutureWatcher&& ohter) = delete;

    /**
     * Deleted move assignemnt operator
     */
    FutureWatcher& operator=(FutureWatcher&& other) = delete;

    /**
     * Sets a future and starts observing the future.
     * @param The future to observe.
     */
    void setFuture(std::future<T> future) noexcept
    {
        mFuture = std::move(future);
        try {
            mFutureObserver = std::thread{[this]() {
                mFuture.wait();
                EventLoop::postEvent(this, std::make_unique<Event>(Event::Type::ThreadFinished));
                mFinished = true;
            }};
        } catch (std::system_error& e) {
            std::cerr << "Failed to create future observer error:" << e.what() << std::endl;
        }
    }

    /**
     * Gives the result of the future. If the still running the function will block and waits until the future is ready.
     * If it failed to request the future, defaulted constructed T will be returned.
     * @return The Result of the underlying future.
     */
    [[nodiscard]] T getResult() noexcept
    {
        try {
            mFinished = false;
            return mFuture.get();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return T{};
        }
    }

    /**
     * @copydoc EventReceiver::handleEvent
     */
    bool handleEvent(Event* event) noexcept override
    {
        if (event->getEventType() == Event::Type::ThreadFinished) {
            finished.emit();
            return true;
        }
        return false;
    }

    /**
     * This signal is emitted when the observed future execution is finished.
     */
    KDBindings::Signal<> finished;

private:
    void futureWatcher() const noexcept
    {
        mFuture.wait();
    }

private:
    std::atomic_bool mFinished{false};
    std::thread mFutureObserver{};
    std::future<T> mFuture{};
};

} // namespace Rapid::System
