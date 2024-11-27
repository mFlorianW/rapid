// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "AsyncResult.hpp"
#include "EventLoop.hpp"

namespace Rapid::System
{

AsyncResult::AsyncResult() = default;
AsyncResult::~AsyncResult() = default;
AsyncResult::AsyncResult(AsyncResult&& other) noexcept = default;
AsyncResult& AsyncResult::operator=(AsyncResult&& other) noexcept = default;

Result AsyncResult::getResult() const noexcept
{
    return mResult;
}

std::string_view AsyncResult::getErrorMessage() const noexcept
{
    return mErrorMsg;
}

void AsyncResult::waitForFinished() noexcept
{
    if (mResult != Result::NotFinished) {
        return;
    }

    while (mResult == Result::NotFinished) {
        EventLoop::instance().processEvents();
    }
}

void AsyncResult::setResult(Result result, std::string const& errorMessage) noexcept
{
    mResult = result;
    mErrorMsg = errorMessage;
    done.emit(this);
}

} // namespace Rapid::System
