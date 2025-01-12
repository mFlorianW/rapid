// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "system/Timer.hpp"
#include <boost/asio.hpp>

namespace Rapid::System::Private
{

class TimerImpl
{
public:
    explicit TimerImpl(Timer& timer);
    ~TimerImpl() noexcept;

    TimerImpl(TimerImpl const&) = delete;
    TimerImpl& operator=(TimerImpl const&) = delete;

    TimerImpl(TimerImpl&&) = delete;
    TimerImpl& operator=(TimerImpl&&) = delete;

    void setTimerInterval(std::chrono::nanoseconds interval) noexcept;

private:
    void onTimeout(boost::system::error_code const& errorCode);
    void setupTimer();

    boost::asio::io_context mIoContext;
    boost::asio::steady_timer mBoostTimer{mIoContext};
    Timer& mTimer;
    std::thread mTimerThread;
    std::chrono::nanoseconds mInterval{0};
    bool mTimerActive = false;
};

} // namespace Rapid::System::Private
