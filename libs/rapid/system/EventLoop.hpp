// SPDX-FileCopyrightText: 2024 - 2025 All contributors
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
 * @brief Provides functions to post, process events and can start an endless event loop.
 *
 * @details The @ref EventLoop is needed for every application based on the Rapid libraries.
 *          In the Rapid application context at least one event loop for the main loop have to exists.
 *          Besides that every thread can have an event loop but it's not necessary if the thread doesn't not need to handle events.
 *          The event loop has a signal @ref EventLoop::wakeUp that is emitted when a even loop has work to do.
 *          This signal can be used to integrate the @ref EventLoop into other event loops e.g. Qt.
 *
 *          Events can only be posted for classes that are derived from @ref EventHandler.
 *          The events are always posted in the thread in which the EventHandler was created.
 *          But it's safe to post events for the @ref EventHandler from any other thread with @ref EventHandler::postEvent() function.
 *
 *          Every @ref EventLoop also has an instance of a ConnectionEvaluator.
 *          The evaluator needs to be used in the deferred connections and is needed for connections where the signal emit is executed from a different thread.
 *          If a deferred connection emit is detected by the ConnectionEvaluator this is going to wake up the event loop and starts event processing.
 *          In the @ref EventLoop event processing is then ConnectionEvaluator evaluated and the slot executed in the correct thread context.
 *
 *  @note The @ref EventLoop::wakeUp signal is only emitted for event loops that are not executed with exec.
 *        For the exec case the event loop is automatically waked up and starts the processing.
 *
 *  Example usage main loop:
 *  @code
 *  int main(...) {
 *      auto eventLoop = Rapid::System::EventLoop::instance();
 *      ... // setup application code
 *      eventLoop::exec();
 *  }
 *  @endcode
 *
 *  Example usage thread loop:
 *  @code
 *  // Function that is called in a different
 *  void foo(){
 *      auto eventLoop = Rapid::System::EventLoop::instance(); // creates the event loop in the thread.
 *      eventLoop.wakeUp.connect([&eventloop]{
 *          eventLoop::processEvents // or alternative just call EventLoop::exec() for the thread.
 *      });
 *      ... // Setup your object functions that event
 *  }
 *  @endcode
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
     * Starts a mainloop, blocks and runs infinite until exit event was posted to the main loop.
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
