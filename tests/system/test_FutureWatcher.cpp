// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "system/FutureWatcher.hpp"
#include "testhelper/CompareHelper.hpp"
#include <catch2/catch_all.hpp>
#include <thread>

using namespace Rapid::System;

namespace
{

void sleepy(std::promise<void> promise)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    promise.set_value();
}

void returny(std::promise<bool> promise)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    promise.set_value(true);
}

} // namespace

SCENARIO("The FutureWatcher shall emit the finish signal when the future finishes.", "[FutureWatcher]")
{
    GIVEN("A initialized FutureWatcher")
    {
        auto finishSignalEmitted = false;
        auto sleepyPromise = std::promise<void>{};
        auto futureWatcher = FutureWatcher<void>{sleepyPromise.get_future()};
        std::ignore = futureWatcher.finished.connect([&finishSignalEmitted]() {
            finishSignalEmitted = true;
        });

        WHEN("Observed operation is finished")
        {
            auto thread = std::thread(&sleepy, std::move(sleepyPromise));
            thread.join();

            THEN("The finished signal should be emitted")
            {
                REQUIRE_COMPARE_WITH_TIMEOUT(finishSignalEmitted, true, std::chrono::milliseconds{1000});
            }
        }
    }
}

SCENARIO("The FutureWatcher shall be to return the result of the observed future.", "[FutureWatcher]")
{
    GIVEN("A initialized FutureWatcher")
    {
        auto returnyPromise = std::promise<bool>{};
        auto futureWatcher = FutureWatcher<bool>{returnyPromise.get_future()};

        WHEN("Observed operation is finished")
        {
            auto thread = std::thread(&returny, std::move(returnyPromise));
            thread.join();

            THEN("The correct value of the future shall be returned")
            {
                REQUIRE(futureWatcher.getResult() == true);
            }
        }
    }
}
