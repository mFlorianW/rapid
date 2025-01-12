// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "TestFile.hpp"
#include "common/JsonDeserializer.hpp"
#include "testhelper/Sessions.hpp"
#include <catch2/catch_all.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace Rapid::Common;
using namespace Rapid::TestHelper;

TEST_CASE("The JsonDeserializer shall deserialize a valid json string into a SessionData")
{
    auto expectedSession = Sessions::getTestSession();
    auto result = JsonDeserializer::Session::deserialize(Sessions::getTestSessionAsJson());
    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    CHECK(result.value().getSessionDate() == expectedSession.getSessionDate());
    CHECK(result.value().getSessionTime() == expectedSession.getSessionTime());
    CHECK(result.value().getLaps() == expectedSession.getLaps());
    REQUIRE(result.value().getTrack() == expectedSession.getTrack());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("The JsonDeserializer shall deserialize a real world json string into a SessionData")
{
    std::ifstream file(TEST_FILE_PATH);
    REQUIRE(file.is_open());
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();
    file.close(); // Close the file
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    auto result = JsonDeserializer::Session::deserialize(fileContent);
    REQUIRE(result.has_value());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("The JsonDeserializer shall deserialize a valid json string into a SessionMetaData")
{
    auto expectedSession = Sessions::getTestSessionMetaData();
    auto result = JsonDeserializer::SessionMetaData::deserialize(Sessions::getTestSessionMetaAsJson());
    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    CHECK(result.value().getSessionDate() == expectedSession.getSessionDate());
    CHECK(result.value().getSessionTime() == expectedSession.getSessionTime());
    REQUIRE(result.value().getTrack() == expectedSession.getTrack());
    // NOLINTEND(bugprone-unchecked-optional-access)
}
