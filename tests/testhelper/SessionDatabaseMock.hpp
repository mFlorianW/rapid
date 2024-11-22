// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <ISessionDatabase.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>

namespace Rapid::TestHelper
{

class SessionDatabaseMock : public Storage::ISessionDatabase
{
public:
    // clang-format off
    MAKE_MOCK(getSessionCount, auto()->std::size_t, override);
    MAKE_MOCK(getSessionByIndex, auto(std::size_t)->std::optional<Common::SessionData>, const noexcept override);
    MAKE_MOCK(getSessionByIndexAsync, auto(std::size_t)->std::shared_ptr<Storage::GetSessionResult>, noexcept);
    MAKE_MOCK(storeSession, auto(Common::SessionData const&)->std::shared_ptr<System::AsyncResult>, override);
    MAKE_MOCK(deleteSession, auto(std::size_t)->void, override);
    // clang-format on
};

} // namespace Rapid::TestHelper
