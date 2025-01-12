// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestCall.hpp"

namespace Rapid::Rest
{

bool RestCall::isFinished() const noexcept
{
    return mResult != RestCallResult::Unknown;
}

RestCallResult RestCall::getResult() const noexcept
{
    return mResult;
}

std::string RestCall::getData() const noexcept
{
    return mRawData;
}

void RestCall::setData(std::string const& data) noexcept
{
    mRawData = data;
}

} // namespace Rapid::Rest
