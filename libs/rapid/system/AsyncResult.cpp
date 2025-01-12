// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "AsyncResult.hpp"
#include "EventLoop.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::System
{

AsyncResult::AsyncResult() = default;
AsyncResult::~AsyncResult() = default;

Result AsyncResult::getResult() const noexcept
{
    std::lock_guard<std::mutex> guard{mMutex};
    return mResult;
}

std::string_view AsyncResult::getErrorMessage() const noexcept
{
    std::lock_guard<std::mutex> guard{mMutex};
    return mErrorMsg;
}

void AsyncResult::waitForFinished() noexcept
{
    if (mThreadId != std::this_thread::get_id()) {
        SPDLOG_ERROR("{} can only be called from the owning thread", __func__);
        return;
    }
    if (mResult != Result::NotFinished) {
        return;
    }
    while (mResult == Result::NotFinished) {
        EventLoop::instance().processEvents();
    }
}

void AsyncResult::setResult(Result result, std::string const& errorMessage) noexcept
{
    try {
        {
            std::lock_guard<std::mutex> guard{mMutex};
            mResult = result;
            mErrorMsg = errorMessage;
        }
        done.emit(this);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Faild to emit done signal. Error. {}", e.what());
    }
}

} // namespace Rapid::System
