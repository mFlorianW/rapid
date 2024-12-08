// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Event.hpp"
#include "EventHandler.hpp"
#include <kdbindings/signal.h>
#include <thread>

namespace Rapid::System
{
class EventQueue;

/**
 * Provides functions to post, process events and can start an event loop.
 */
class EventLoop final
{

public:
    /**
     * @brief Gives the for the event loop for the calling thread.
     */
    static EventLoop& instance() noexcept;

    /**
     * Default destructor
     */
    ~EventLoop();

    /**
     * Disabled copy consturctor
     */
    EventLoop(EventLoop const&) = delete;

    /**
     * Disabled move constructor
     */
    EventLoop(EventLoop&&) noexcept = delete;

    /**
     * Disabled move operator
     */
    EventLoop& operator=(EventLoop const&) = delete;

    /**
     * Disabled copy assignment operator
     */
    EventLoop& operator=(EventLoop&&) noexcept = delete;

    /**
     * Post an event for the receiver
     * @param receiver The receiver that shall receive the event.
     */
    static void postEvent(EventHandler* receiver, std::unique_ptr<Event> event);

    /**
     * Checks if for an existing event of @ref Event::Type type in the event loop
     * @return true Event is present, otherwise false.
     */
    bool isEventQueued(EventHandler* receiver, Event::Type type) const noexcept;

    /**
     * Process all queue events and then returns
     */
    void processEvents();

    /**
     * Starts a mainloop, blocks and runs infinite until exit was called.
     */
    void exec();

    /**
     * Stops and quits an endless running eventloop
     */
    void quit() noexcept;

    /**
     * @brief Gives the ConnectionEvaluator for the calling thread.
     *
     * @details With the help of the ConnectionEvaluator the slots are executed in the correct thread from signals that are emitted in a different thread.
     *          The ConnectionEvaluator must be used when a connection to a signal is done with "connectDeferred".
     *          The "connectDeferred" defers the slot execution until the @ref EventLoop of the slot is executed.
     *          For that reason the thread of the slot must have an @ref EventLoop and @ref EventLoop::ProcessEvents must be called.
     */
    static std::shared_ptr<KDBindings::ConnectionEvaluator> getConnectionEvaluator();

    /**
     * @brief This signal is emitted when for the event loop an something to do.
     *
     * @details The main purpose for this signal is integration with other event loops
     *          e.g. Qt. Here is the signal used to wake up Qt event loop that it process
     *          the events from the rapid event loop.
     *          Usually there no need to connect to this signal.
     *          The signal is only emitted when the event loop is not started with @ref EventLoop::exec.
     */
    KDBindings::Signal<> wakeUp;

protected:
    /**
     * Default consturctor
     */
    EventLoop(EventQueue& queue);

private:
    friend class Rapid::System::EventHandler;
    friend std::unique_ptr<Rapid::System::EventLoop>;

    /**
     * Clear all events for a specific event handler
     */
    static void clearEvents(EventHandler* eventHandler) noexcept;

    static std::unordered_map<std::thread::id, std::unique_ptr<EventLoop>> mEventLoops;

    std::thread::id mOwningThread;
    EventQueue& mEventQueue;
};

} // namespace Rapid::System
