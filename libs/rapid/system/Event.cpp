// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Event.hpp"

namespace Rapid::System
{

Event::Event(Type type)
    : mEventType{type}
{
}

Event::~Event() noexcept = default;

Event::Type Event::getEventType() const noexcept
{
    return mEventType;
}

} // namespace Rapid::System
