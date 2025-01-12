// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SIGNALSPY_HPP
#define SIGNALSPY_HPP

#include "kdbindings/connection_handle.h"

namespace Rapid::TestHelper
{

template <typename... Args>
class SignalSpy
{
public:
    using SignalType = KDBindings::Signal<Args...>;
    using SignalArgs = std::tuple<Args...>;

    SignalSpy(SignalType& signal)
    {
        mConnectionHandle = signal.connect([this](Args... args) {
            mValues.insert({mSignalCount, SignalArgs{args...}});
            ++mSignalCount;
        });
    }

    ~SignalSpy()
    {
        mConnectionHandle.disconnect();
    }

    SignalSpy(SignalSpy const& other) = delete;
    SignalSpy& operator=(SignalSpy const& other) = delete;

    SignalSpy(SignalSpy&& other) = delete;
    SignalSpy& operator=(SignalSpy&& other) = delete;

    std::size_t getCount() const noexcept
    {
        return mSignalCount;
    }

    SignalArgs at(std::size_t index)
    {
        return mValues.at(index);
    }

private:
    KDBindings::ConnectionHandle mConnectionHandle;
    std::size_t mSignalCount = 0U;
    std::unordered_map<std::size_t, SignalArgs> mValues;
};

} // namespace Rapid::TestHelper

#endif // SIGNALSPY_HPP
