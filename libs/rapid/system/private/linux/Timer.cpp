// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Timer.hpp"
#include "system/EventLoop.hpp"
#include <ctime>
#include <spdlog/spdlog.h>
#include <sys/timerfd.h>

namespace Rapid::System::Private::Linux
{

Timer::Timer(Rapid::System::Timer* timer)
    : mTimer{timer}
    , mTimerFd(timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC))
    , mTimerNotifier{mTimerFd, FdNotifierType::Read}
{
    mNotifierConnection = mTimerNotifier.notify.connect([this]() {
        std::array<std::uint8_t, 8> timeouts{};
        std::ignore = read(mTimerFd, timeouts.data(), timeouts.size());
        System::EventLoop::instance().postEvent(mTimer, std::make_unique<Event>(Event::Type::Timeout));
    });
}

void Timer::setTimerInterval(std::chrono::nanoseconds interval) noexcept
{
    bool useSeconds = false;
    if (interval >= std::chrono::seconds{1}) {
        useSeconds = true;
    }
    auto timerConfig = itimerspec{};
    if (interval > std::chrono::nanoseconds{0}) {
        timerConfig.it_interval.tv_sec = useSeconds ? interval.count() / 1'000'000'000 : 0;
        timerConfig.it_interval.tv_nsec = not useSeconds ? interval.count() : 0;
        timerConfig.it_value.tv_sec = useSeconds ? interval.count() / 1'000'000'000 : 0;
        timerConfig.it_value.tv_nsec = not useSeconds ? interval.count() : 0;
    } else {
        timerConfig.it_interval.tv_sec = 0;
        timerConfig.it_interval.tv_nsec = 0;
        timerConfig.it_value.tv_sec = 0;
        timerConfig.it_value.tv_nsec = 0;
    }
    if (timerfd_settime(mTimerFd, 0, &timerConfig, nullptr) < 0) {
        SPDLOG_ERROR("Failed to setup timer. Error: {}", strerror(errno));
    }
}

} // namespace Rapid::System::Private::Linux
