// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include "JsonSerializer.hpp"
#include "Sessions.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;
using namespace Rapid::TestHelper;

TEST_CASE("Serialize a json to a json string")
{
    auto const session = Sessions::getTestSession();
    std::string result = JsonSerializer::Session::serialize(session);
    REQUIRE(result == Sessions::getTestSessionAsJson());
}
