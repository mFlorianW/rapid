// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "EventHandler.hpp"
#include "EventLoop.hpp"

namespace Rapid::System
{

EventHandler::EventHandler()
    : mThreadId{std::this_thread::get_id()}
{
}

EventHandler::~EventHandler() noexcept
{
    EventLoop::clearEvents(this);
}

std::thread::id EventHandler::getThreadId() const noexcept
{
    return mThreadId;
}

} // namespace Rapid::System
