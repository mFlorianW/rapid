// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TIMER_HPP
#define TIMER_HPP

#include "EventHandler.hpp"
#include <chrono>
#include <kdbindings/signal.h>

namespace Rapid::System
{

namespace Private
{
class TimerImpl;
}

class Timer final : public EventHandler
{
public:
    /**
     * Creates an instance of the Timer.
     */
    Timer();

    /**
     * Default destructor
     */
    ~Timer() override;

    /**
     * Deleted copy constructor
     */
    Timer(Timer const& other) = delete;

    /**
     * Deleted copy operator
     */
    Timer& operator=(Timer const& other) = delete;

    /**
     * Deleted move constructor
     */
    Timer(Timer&& other) = delete;

    /**
     * Deleted move operator
     */
    Timer& operator=(Timer&& other) = delete;

    /**
     * Starts the timer. The timer sends the timeout signal when the set interval is reached.
     */
    void start();

    /**
     * Stops the timer.
     */
    void stop();

    /**
     * Sets the interval for the timer.
     * Setting the interval for a already running timer, stops and starts the timer again with the new interval.
     */
    void setInterval(std::chrono::milliseconds interval);

    /**
     * Gives the interval of the timer.
     * @return Gives the interval of the timer.
     */
    std::chrono::milliseconds getInterval();

    /**
     * Gives the state of the timer is it running or not.
     * @return True means the timer is running, false means is not.
     */
    bool isRunning();

    /**
     * @copydoc EventReceiver::handleEvent
     */
    bool handleEvent(Event* event) override;

    /**
     * This signal is emitted when the timer reaches it's intervall.
     */
    KDBindings::Signal<> timeout;

private:
    std::chrono::milliseconds mInterval{0};
    bool mRunning{false};
    std::unique_ptr<Private::TimerImpl> mTimer;
};

} // namespace Rapid::System

#endif // TIMER_HPP
