// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_TESTHELPER_ACTIVESESSIONMOCK_HPP
#define RAPID_TESTHELPER_ACTIVESESSIONMOCK_HPP

#include <trompeloeil.hpp>
#include <workflow/IActiveSessionWorkflow.hpp>

namespace Rapid::TestHelper
{
class ActiveSessionMock : public Workflow::IActiveSessionWorkflow
{
public:
    MAKE_MOCK(startActiveSession, auto()->void, noexcept override);
    MAKE_MOCK(stopActiveSession, auto()->void, noexcept override);
    MAKE_MOCK(setTrack, auto(Common::TrackData const&)->void, noexcept override);
    MAKE_MOCK(getTrack, auto()->std::optional<Common::TrackData>, const noexcept override);
    MAKE_MOCK(getSession, auto()->std::optional<Common::SessionData>, const noexcept override);
};

} // namespace Rapid::TestHelper

#endif // !RAPID_TESTHELPER_ACTIVESESSIONMOCK_HPP
