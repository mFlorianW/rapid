// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Timer.hpp"
#ifdef __linux__
#include "system/private/linux/Timer.hpp"
#elif
#error "Unsupported platform"
#endif

namespace Rapid::System
{
#ifdef __linux__

class TimerPrivate
{
public:
    TimerPrivate(Timer* timer)
        : mQ{timer}
    {
    }

    Timer* mQ;
    std::chrono::milliseconds mInterval{0};
    bool mRunning{false};
    Private::Linux::Timer mTimer{mQ};
};

#endif

Timer::Timer()
    : mD{std::make_unique<TimerPrivate>(this)}
{
}

Timer::~Timer()
{
    if (mD->mRunning) {
        stop();
    }
}

void Timer::start()
{
    mD->mTimer.setTimerInterval(std::chrono::duration_cast<std::chrono::nanoseconds>(mD->mInterval));
    mD->mRunning = true;
}

void Timer::stop()
{
    mD->mTimer.setTimerInterval(std::chrono::nanoseconds(0));
    mD->mRunning = false;
}

void Timer::setInterval(std::chrono::milliseconds interval)
{
    mD->mInterval = interval;
}

std::chrono::milliseconds Timer::getInterval()
{
    return mD->mInterval;
}

bool Timer::isRunning()
{
    return mD->mRunning;
}

bool Timer::handleEvent(Event* event)
{
    if (mD->mRunning and event->getEventType() == Event::Type::Timeout) {
        timeout.emit();
        return true;
    }
    return false;
}

} // namespace Rapid::System
