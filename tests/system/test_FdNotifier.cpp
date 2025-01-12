// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <fcntl.h>
#include <filesystem>
#include <sys/stat.h>
#include <system/FdNotifier.hpp>
#include <testhelper/CompareHelper.hpp>
#include <testhelper/SignalSpy.hpp>
#include <unistd.h>

using namespace Rapid::System;
using namespace Rapid::TestHelper;
using namespace std::chrono_literals;

constexpr auto timeout = 1s;

struct TestFixture
{
    int fd = -1;

    TestFixture()
    {
        std::string filePath = std::string{"/tmp/rapid"}.append(Catch::getResultCapture().getCurrentTestName());
        if (std::filesystem::exists(filePath) && remove(filePath.c_str()) < 0) {
            FAIL("Failed to remove temp test file");
        }
        auto fifoResult = mkfifo(filePath.c_str(), 0600);
        if (fifoResult < 0) {
            FAIL(strerror(errno));
        }
        fd = open(filePath.c_str(), O_RDWR | O_NONBLOCK);
        if (fd < 0) {
            FAIL(strerror(errno));
        }
        int sizeResult = fcntl(fd, F_SETPIPE_SZ, 1024);
        if (sizeResult < 0) {
            FAIL(strerror(errno));
        }
    }
};

TEST_CASE_METHOD(TestFixture, "The FdNotifier shall notify when data to be read")
{
    auto fdNotifier = FdNotifier{fd, FdNotifierType::Read};
    auto notifySpy = SignalSpy{fdNotifier.notify};

    auto buffer = std::uint8_t{10};
    write(fd, &buffer, sizeof(buffer));
    REQUIRE_COMPARE_WITH_TIMEOUT(notifySpy.getCount(), 1, timeout);
    auto [signalFd, type] = notifySpy.at(0);
    REQUIRE(signalFd == fd);
    REQUIRE(type == FdNotifierType::Read);
}

TEST_CASE_METHOD(TestFixture, "The FdNotifier shall notify when is ready to write")
{
    auto fdNotifier = FdNotifier{fd, FdNotifierType::Write};
    auto notifySpy = SignalSpy{fdNotifier.notify};
    REQUIRE_COMPARE_WITH_TIMEOUT(notifySpy.getCount(), 1, timeout);
    auto [signalFd, type] = notifySpy.at(0);
    REQUIRE(signalFd == fd);
    REQUIRE(type == FdNotifierType::Write);
}

TEST_CASE_METHOD(TestFixture, "The FdNotifier shall support READ and WRITE for one file descriptor")
{
    auto readNotifier = FdNotifier{fd, FdNotifierType::Read};
    auto readNotifierSpy = SignalSpy{readNotifier.notify};
    auto writeNotifier = FdNotifier{fd, FdNotifierType::Write};
    auto writeNotifierSpy = SignalSpy{writeNotifier.notify};

    REQUIRE_COMPARE_WITH_TIMEOUT(writeNotifierSpy.getCount(), 1, timeout);

    auto buffer = std::uint8_t{};
    auto bytes = write(fd, &buffer, sizeof(buffer));
    REQUIRE(bytes == 1);
    REQUIRE_COMPARE_WITH_TIMEOUT(readNotifierSpy.getCount(), 1, timeout);
}
