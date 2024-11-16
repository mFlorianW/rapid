// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <FileSystemSessionDatabaseBackend.hpp>
#include <Sessions.hpp>
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <unistd.h>

using namespace Rapid::Storage;
using namespace Rapid::TestHelper;

namespace
{

std::string getTestDatabseFolder()
{
    std::array<char, 512> buffer{};

    auto* bufferPtr = getcwd(&buffer.at(0), buffer.size());
    if (bufferPtr == nullptr) {
        FAIL("Unable to get the database folder.");
    }

    auto dbDir = std::string{&buffer.at(0)} + "/database";

    return dbDir;
}

class FileSystemTestEventListener : public Catch::EventListenerBase
{
public:
    using Catch::EventListenerBase::EventListenerBase;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        auto dbFolder = getTestDatabseFolder();

        // For the case the test crashes.
        if (std::filesystem::exists(dbFolder)) {
            std::filesystem::remove_all(dbFolder);
        }

        std::filesystem::create_directory(dbFolder);
    }

    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override
    {
        auto dbFolder = getTestDatabseFolder();
        std::filesystem::remove_all(dbFolder);
    }
};
CATCH_REGISTER_LISTENER(FileSystemTestEventListener);

} // namespace

TEST_CASE("The FileSystemSessionDatabaseBackend shall store the first session under session1.json")
{
    auto dbFolder = getTestDatabseFolder();
    auto fsBackend = FileSystemSessionDatabaseBackend{dbFolder};
    auto expectedFilePath = std::filesystem::path{dbFolder + "/session1.json"};

    REQUIRE(fsBackend.storeSession(1, Sessions::getTestSessionAsJson()));
    REQUIRE(std::filesystem::exists(expectedFilePath));
}

TEST_CASE("The FileSystemSessionDatabaseBackend store shall fail when database folder doesn't exists.")
{
    auto dbFolder = getTestDatabseFolder();
    auto fsBackend = FileSystemSessionDatabaseBackend{dbFolder};

    std::filesystem::remove_all(dbFolder);

    REQUIRE(!fsBackend.storeSession(1, Sessions::getTestSessionAsJson()));
}

TEST_CASE("The FileSystemSessionDatabaseBackend shall be able to load a session by it's index")
{
    auto dbFolder = getTestDatabseFolder();
    auto fsBackend = FileSystemSessionDatabaseBackend{dbFolder};

    // create Session
    auto testSessionFile = std::ofstream{dbFolder + "/" + "session1.json"};
    testSessionFile << Sessions::getTestSessionAsJson();
    testSessionFile.close();

    auto loadedSession = fsBackend.loadSessionByIndex(1);

    REQUIRE(loadedSession == Sessions::getTestSessionAsJson());
}

TEST_CASE("The FileSystemSessionDatabaseBackend shall return the last stored index")
{
    auto dbFolder = getTestDatabseFolder();
    auto fsBackend = FileSystemSessionDatabaseBackend{dbFolder};

    REQUIRE(fsBackend.storeSession(1, Sessions::getTestSessionAsJson()));
    REQUIRE(fsBackend.getLastStoredIndex() == 1);
}

TEST_CASE("The FileSystemSessionDatabaseBackend shall return a list of all stored sessions")
{
    auto dbFolder = getTestDatabseFolder();
    auto fsBackend = FileSystemSessionDatabaseBackend{dbFolder};
    auto expectedIndexList = std::vector<std::size_t>{1, 5};

    // create Session
    auto testSessionFile = std::ofstream{dbFolder + "/" + "session1.json"};
    testSessionFile << Sessions::getTestSessionAsJson();
    testSessionFile.close();

    auto testSessionFile2 = std::ofstream{dbFolder + "/" + "session5.json"};
    testSessionFile << Sessions::getTestSessionAsJson();
    testSessionFile.close();

    REQUIRE(fsBackend.getIndexList() == expectedIndexList);
}

TEST_CASE("The FileSystemSessionDatabaseBackend shall return the number of stored session")
{
    auto dbFolder = getTestDatabseFolder();
    auto fsBackend = FileSystemSessionDatabaseBackend{dbFolder};
    auto expectedIndexList = std::vector<std::size_t>{1, 5};

    // create Session
    auto testSessionFile = std::ofstream{dbFolder + "/" + "session1.json"};
    testSessionFile << Sessions::getTestSessionAsJson();
    testSessionFile.close();

    auto testSessionFile2 = std::ofstream{dbFolder + "/" + "session5.json"};
    testSessionFile << Sessions::getTestSessionAsJson();
    testSessionFile.close();

    auto testSessionFile3 = std::ofstream{dbFolder + "/" + "session10.json"};
    testSessionFile << Sessions::getTestSessionAsJson();
    testSessionFile.close();

    REQUIRE(fsBackend.getNumberOfStoredSessions() == 3);
}

TEST_CASE(
    "The FileSystemSessionDatabaseBackend shall return the last stored index after creation without storing anything.")
{
    auto dbFolder = getTestDatabseFolder();
    // Create two empty files. Files are created when the ofstream is running out of scope.
    {
        std::ofstream file1{dbFolder + "/session1.json"};
        std::ofstream file2{dbFolder + "/session2.json"};
    }
    auto fsBackend = FileSystemSessionDatabaseBackend{dbFolder};
    REQUIRE(fsBackend.getLastStoredIndex() == 2);
}
