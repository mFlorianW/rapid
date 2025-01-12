// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <cstdint>

namespace Rapid::System
{

/**
 * The base class for every event which can occur in the Rapid sytstem.
 */
class Event
{
public:
    /**
     * Defines the events in the Rapid system
     */
    enum class Type : std::uint16_t
    {
        Unknown,
        Timeout,
        ThreadFinished,
        HttpRequestReceived,
        Notifier,
    };

    /**
     * Creates an Event instance
     */
    Event(Type type = Type::Unknown);

    /**
     * Default destructor
     */
    virtual ~Event() noexcept;

    /**
     * Disabled copy constructor
     */
    Event(Event const&) = delete;

    /**
     * Disabled copy operator
     */
    Event& operator=(Event const&) = delete;

    /**
     * Move constructor, moves one event to a new instance.
     */
    Event(Event&&) noexcept = default;

    /**
     * Move operator, moves one event to a new instance
     */
    Event& operator=(Event&&) noexcept = default;

    /**
     * @return Gives the type of the event
     */
    Type getEventType() const noexcept;

private:
    Type mEventType;
};

} // namespace Rapid::System
