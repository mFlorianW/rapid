// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_SYSTEM_PRIVATE_LINUX_TIMER_HPP
#define RAPID_SYSTEM_PRIVATE_LINUX_TIMER_HPP

#include "FdNotifierImpl.hpp"
#include "system/Timer.hpp"
#include <chrono>

namespace Rapid::System::Private::Linux
{

class Timer
{
public:
    Timer(Rapid::System::Timer* timer);

    void setTimerInterval(std::chrono::nanoseconds interval) noexcept;

private:
    Rapid::System::Timer* mTimer;
    int mTimerFd = -1;
    FdNotifier mTimerNotifier;
    KDBindings::ScopedConnection mNotifierConnection;
};

} // namespace Rapid::System::Private::Linux

#endif // !RAPID_SYSTEM_PRIVATE_LINUX_TIMER_HPP
