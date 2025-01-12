// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Timer.hpp"
#ifdef WITH_BOOST
#include "private/BoostTimer.hpp"
#else
#include "private/PosixTimer.hpp"
#endif

namespace Rapid::System
{

Timer::Timer()
    : mTimer{std::make_unique<Private::TimerImpl>(*this)}
{
}

Timer::~Timer()
{
    if (mRunning) {
        stop();
    }
}

void Timer::start()
{
    mTimer->setTimerInterval(std::chrono::duration_cast<std::chrono::nanoseconds>(mInterval));
    mRunning = true;
}

void Timer::stop()
{
    mTimer->setTimerInterval(std::chrono::nanoseconds(0));
    mRunning = false;
}

void Timer::setInterval(std::chrono::milliseconds interval)
{
    mInterval = interval;
}

std::chrono::milliseconds Timer::getInterval()
{
    return mInterval;
}

bool Timer::isRunning()
{
    return mRunning;
}

bool Timer::handleEvent(Event* event)
{
    if (mRunning and event->getEventType() == Event::Type::Timeout) {
        timeout.emit();
        return true;
    }
    return false;
}

} // namespace Rapid::System
