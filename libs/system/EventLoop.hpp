// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Event.hpp"
#include "EventHandler.hpp"
#include <kdbindings/signal.h>
#include <memory>
#include <thread>

namespace Rapid::System
{

/**
 * Provides functions to post, process events and can start an event loop.
 */
class EventLoop final
{
public:
    /**
     * @brief Gives the for the event loop for the calling thread.
     */
    static EventLoop& instance();

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
    EventLoop& operator=(EventLoop&&) noexcept = default;

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
     * @brief This signal is emitted when for the event loop an event was posted
     *
     * @details The main purpose for this signal is integration with other event loops
     *          e.g. Qt. Here is the signal used to wake up Qt event loop that it process
     *          the events from the rapid event loop.
     *          Usually there no need to connect to this signal.
     */
    KDBindings::Signal<> eventPosted;

private:
    friend class Rapid::System::EventHandler;

    /**
     * Default consturctor
     */
    EventLoop();

    /**
     * Clear all events for a specific event handler
     */
    static void clearEvents(EventHandler* eventHandler) noexcept;

    std::thread::id mOwningThread;
};

} // namespace Rapid::System
