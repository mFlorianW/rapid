// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DummyProcessPath.hpp"
#include <ProcessManager.hpp>
#include <QSignalSpy>
#include <QTest>
#include <catch2/catch_all.hpp>
#include <system/qt/RapidApplication.hpp>
#include <testhelper/CompareHelper.hpp>
#include <testhelper/qt/ProcessStatusChecker.hpp>
#include <testhelper/qt/ProcessStopper.hpp>
#include <testhelper/qt/QtTestHelper.hpp>

using namespace Rapid::RapidShell;

SCENARIO("The ProcessManager shall start an process.")
{
    GIVEN("An initialzed ProcessManager")
    {
        auto processMgr = ProcessManager{};
        auto errorSpy = QSignalSpy{&processMgr, &ProcessManager::processError};
        WHEN("A process is started")
        {
            processMgr.startProcess(DUMMY_PROCESS_BINARY_PATH);
            THEN("The process shall run")
            {
                REQUIRE(Rapid::TestHelper::ProcessStatusChecker{}.isProcessRunning(DUMMY_PROCESS_BINARY_NAME));
            }
        }

        WHEN("An invalid process name is passed")
        {
            auto const processName = QStringLiteral("not_existing_process");
            processMgr.startProcess(processName);
            THEN("The process start function shall send an error message")
            {
                QTRY_COMPARE_WITH_TIMEOUT(errorSpy.size(), 1, std::chrono::seconds{1}.count());
                auto params = errorSpy.takeFirst();
                REQUIRE(params.at(0).toString() == processName);
                REQUIRE(params.at(1).value<ProcessManager::Error>() == ProcessManager::Error::ProcessNotFound);
            }
        }
    }
}

SCENARIO("The ProcessManager shall check for running processes")
{
    GIVEN("An initialzed ProcessManager")
    {
        auto processMgr = ProcessManager{};
        WHEN("A process is not started")
        {
            THEN("The running process check shall be negativ")
            {
                REQUIRE(processMgr.isProcessRunning(DUMMY_PROCESS_BINARY_NAME) == false);
            }
        }

        WHEN("A process is started")
        {
            processMgr.startProcess(DUMMY_PROCESS_BINARY_PATH);
            THEN("The running process check shall be positiv")
            {
                REQUIRE(processMgr.isProcessRunning(DUMMY_PROCESS_BINARY_NAME) == true);
            }
        }

        WHEN("A process is started")
        {
            processMgr.startProcess(DUMMY_PROCESS_BINARY_PATH);
            THEN("The running process check with an absolute file path shall be positiv")
            {
                REQUIRE(processMgr.isProcessRunning(DUMMY_PROCESS_BINARY_PATH) == true);
            }
        }
    }
}

SCENARIO("The process manager shall restart a stopped application")
{
    GIVEN("An initialzed ProcessManager")
    {
        auto processMgr = ProcessManager{};
        WHEN("A running process is stopped and started again")
        {
            processMgr.startProcess(DUMMY_PROCESS_BINARY_PATH);
            REQUIRE(Rapid::TestHelper::ProcessStopper::stopProcess(DUMMY_PROCESS_BINARY_NAME));
            REQUIRE(QTest::qWaitFor(
                [] {
                    return Rapid::TestHelper::ProcessStatusChecker{}.isProcessRunning(DUMMY_PROCESS_BINARY_NAME) ==
                           false;
                },
                std::chrono::milliseconds{10000}.count()));
            processMgr.startProcess(DUMMY_PROCESS_BINARY_PATH);
            THEN("The process shall be restarted again")
            {
                REQUIRE(Rapid::TestHelper::ProcessStatusChecker{}.isProcessRunning(DUMMY_PROCESS_BINARY_NAME));
            }
        }
    }
}

QT_CATCH2_TEST_MAIN()
