// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "PosixTimer.hpp"
#include "EventLoop.hpp"
#include <csignal>
#include <cstring>
#include <spdlog/spdlog.h>

namespace Rapid::System::Private
{

namespace
{

void handler(sigval_t val)
{
    auto timer = static_cast<Timer*>(val.sival_ptr);
    EventLoop::postEvent(timer, std::make_unique<Event>(Event::Type::Timeout));
}

} // namespace

TimerImpl::TimerImpl(Timer& timer) noexcept
    : mTimer{timer}
{
    auto timerEv = sigevent{};
    timerEv.sigev_value.sival_ptr = &mTimer;
    timerEv.sigev_notify_function = &handler;
    timerEv.sigev_notify = SIGEV_THREAD;

    auto result = timer_create(CLOCK_REALTIME, &timerEv, &mTimerId);
    if (result != 0) {
        spdlog::error("Failed to create timer posix timer. Error: {}", strerror(errno));
    }

    mInitialized = true;
}

TimerImpl::~TimerImpl() noexcept
{
    if (mInitialized) {
        setTimerInterval(std::chrono::nanoseconds{0});
    }
}

void TimerImpl::setTimerInterval(std::chrono::nanoseconds interval) noexcept
{
    if (not mInitialized) {
        spdlog::error("Timer can't be started. Error: timer not initialized");
        return;
    }

    auto timerSpec = itimerspec{};
    timerSpec.it_value.tv_nsec = interval.count();
    timerSpec.it_value.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = interval.count();
    timerSpec.it_interval.tv_sec = 0;

    auto result = timer_settime(mTimerId, 0, &timerSpec, nullptr);
    if (result != 0) {
        spdlog::error("Failed to activate timer. Timer id: {} Error: {}", mTimerId, strerror(errno));
    }
}

} // namespace Rapid::System::Private
