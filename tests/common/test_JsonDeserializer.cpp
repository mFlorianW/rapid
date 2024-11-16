// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "JsonDeserializer.hpp"
#include "Sessions.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;
using namespace Rapid::TestHelper;

TEST_CASE("The JsonDeserializer shall deserialize a valid json string into a SessionData")
{
    auto expectedSession = Sessions::getTestSession();
    auto result = JsonDeserializer::deserializeSessionData(Sessions::getTestSessionAsJson());
    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result.value() == expectedSession);
    // NOLINTEND(bugprone-unchecked-optional-access)
}
