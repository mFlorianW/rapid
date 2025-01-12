// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SqliteDatabaseTestHelper.hpp"
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <filesystem>

namespace Rapid::TestHelper::SqliteDatabaseTestHelper
{

std::string getWorkingDir()
{
    auto buffer = std::array<char, 512>{0};
    auto* bufferPtr = getcwd(&buffer.at(0), buffer.size());
    if (bufferPtr == nullptr) {
        FAIL("Unable to get the database folder.");
    }

    return std::string{&buffer.at(0)};
}

std::string getTestDatabseFolder()
{
    return getWorkingDir() + "/database";
}

std::string getDbNameForTest() noexcept
{
    return Catch::getResultCapture().getCurrentTestName() + ".db";
}

std::string getTestDatabaseFile(std::string const& dbFile)
{
    return getTestDatabseFolder() + "/" + dbFile;
}

} // namespace Rapid::TestHelper::SqliteDatabaseTestHelper

namespace Rapid::TestHelper
{

void SqliteDatabaseTestEventlistener::sectionStarting(Catch::SectionInfo const& sectionInfo)
{
    cleanDatabase(SqliteDatabaseTestHelper::getDbNameForTest());
}

void SqliteDatabaseTestEventlistener::cleanDatabase(std::string const& dbName)
{
    if (std::filesystem::exists(SqliteDatabaseTestHelper::getTestDatabseFolder())) {
        REQUIRE(std::filesystem::remove_all(SqliteDatabaseTestHelper::getTestDatabseFolder()));
    }

    if (not std::filesystem::exists(SqliteDatabaseTestHelper::getTestDatabseFolder())) {
        REQUIRE(std::filesystem::create_directory(SqliteDatabaseTestHelper::getTestDatabseFolder()));
    }

    auto const dbFile = SqliteDatabaseTestHelper::getTestDatabseFolder() + "/" + dbName;
    auto const cleanDbFile = SqliteDatabaseTestHelper::getWorkingDir() + "/" + getCleanDbFileName();
    REQUIRE(std::filesystem::copy_file(cleanDbFile, dbFile));
}

} // namespace Rapid::TestHelper
