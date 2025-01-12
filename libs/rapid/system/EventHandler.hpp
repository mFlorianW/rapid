// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "Event.hpp"
#include <thread>

namespace Rapid::System
{

/**
 * Base classes that wants to handle events.
 * If for the class an events occurs the @ref EventHandler::handleEvent function is called.
 * Events are always handled in the thread context in which the handler was created.
 */
class EventHandler
{
public:
    /**
     * Default Destructor
     */
    virtual ~EventHandler() noexcept;

    /**
     * Disabled copy consturctor
     */
    EventHandler(EventHandler const&) = delete;

    /**
     * disabled copy assignment operator
     */
    EventHandler& operator=(EventHandler const&) = delete;

    /**
     * Disabled move consturctor
     */
    EventHandler(EventHandler&&) noexcept = default;

    /**
     * Disabled move assignment operator
     */
    EventHandler& operator=(EventHandler&&) noexcept = default;

    /**
     * That function is called when an event for the handler occurs
     * @param The event for the handler
     * @return true event handled, otherwise false.
     */
    virtual bool handleEvent(Event* event) = 0;

    /**
     * Gives the thread id of the event handler
     * This function is mainly used to handle the event in the correct
     * thread context.
     * @return The thread id of the handler.
     */
    std::thread::id getThreadId() const noexcept;

protected:
    EventHandler();

private:
    std::thread::id mThreadId = std::thread::id{0};
};

} // namespace Rapid::System
