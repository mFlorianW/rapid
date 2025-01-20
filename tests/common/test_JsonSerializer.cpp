// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <common/JsonSerializer.hpp>
#include <testhelper/Sessions.hpp>
#include <testhelper/Tracks.hpp>

using namespace Rapid::Common;
using namespace Rapid::TestHelper;

TEST_CASE("Serialize a session to a json string", "[JSONSERIALIZER_SESSION]")
{
    auto const session = Sessions::getTestSession();
    std::string result = JsonSerializer::Session::serialize(session);
    REQUIRE(result == Sessions::getTestSessionAsJson());
}

TEST_CASE("Serialize a sessionmetadata to a json string", "[JSONSERIALIZER_SESSION]")
{
    auto const session = Sessions::getTestSessionMetaData();
    std::string result = JsonSerializer::Session::serialize(session);
    REQUIRE(result == Sessions::getTestSessionMetaAsJson());
}

TEST_CASE("Serialize a trackdata to a json string", "[JSONSERIALIZER_TRACK]")
{
    auto jsonTrack = JsonSerializer::Track::serialize(Tracks::getTrack());
    REQUIRE(jsonTrack == Tracks::getTrackAsJson());
}
