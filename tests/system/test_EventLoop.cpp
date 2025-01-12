// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#define CATCH_CONFIG_MAIN
#include "system/Event.hpp"
#include "system/EventLoop.hpp"
#include <catch2/catch_all.hpp>
#include <testhelper/CompareHelper.hpp>
#include <thread>

using namespace Rapid::System;

class TestEventReceiver : public EventHandler
{

public:
    bool eventReceived = false;
    std::thread::id tid = std::thread::id{0};
    std::size_t handleEventCallCount = 0;

    bool handleEvent(Event* e) override
    {
        tid = std::this_thread::get_id();
        eventReceived = true;
        ++handleEventCallCount;
        return true;
    }
};

SCENARIO("An EventLoop shall call the receiver of an Event.")
{
    GIVEN("An Eventloop and EventReceiver")
    {
        auto& eventLoop = EventLoop::instance();
        auto eventReceiver = TestEventReceiver{};
        WHEN("An event is posted to the loop and process events")
        {
            eventLoop.postEvent(&eventReceiver, std::make_unique<Event>());
            eventLoop.processEvents();
            THEN("The event shall be delivered to the receiver")
            {
                REQUIRE(eventReceiver.eventReceived);
            }
        }
    }
}

SCENARIO("Process posted events from a different thread in the receivers EventLoop")
{
    GIVEN("An EventLoop and EventReceiver")
    {
        auto& eventLoop = EventLoop::instance();
        auto eventReceiver = TestEventReceiver{};
        auto const threadId = std::this_thread::get_id();
        WHEN("Posting event from a different thread")
        {
            auto thread = std::thread{[&eventLoop, &eventReceiver]() {
                eventLoop.postEvent(&eventReceiver, std::make_unique<Event>());
            }};
            thread.join();
            eventLoop.processEvents();
            THEN("Event function is called in the receivers thread")
            {
                REQUIRE(eventReceiver.tid == threadId);
            }
        }
    }
}

SCENARIO("Process events when called from the owning thread.")
{
    GIVEN("An Eventloop and EventReceiver")
    {
        auto& eventLoop = EventLoop::instance();
        auto eventReceiver = TestEventReceiver{};
        WHEN("Process events from a different thread")
        {
            auto thread = std::thread{[&eventLoop, &eventReceiver]() {
                eventLoop.postEvent(&eventReceiver, std::make_unique<Event>());
                eventLoop.processEvents();
            }};
            thread.join();
            THEN("The eventloop doesn't process events")
            {
                REQUIRE(eventReceiver.eventReceived == false);
            }
        }
    }
}

SCENARIO("Every thread shall have it's own event loop and events for a specific thread shall be processed")
{
    GIVEN("Threads with EventLoops")
    {
        auto tid1 = std::thread::id{0};
        auto tid11 = std::thread::id{0};
        auto thread1 = std::thread{[&tid1, &tid11]() {
            auto& eventLoop = EventLoop::instance();
            auto eventReceiver = TestEventReceiver();
            eventLoop.postEvent(&eventReceiver, std::make_unique<Event>());
            eventLoop.processEvents();
            tid1 = eventReceiver.tid;
            tid11 = std::this_thread::get_id();
        }};
        if (thread1.joinable()) {
            thread1.join();
        } else {
            FAIL("Thread1 is not joinable.");
        }

        auto tid2 = std::thread::id{0};
        auto tid22 = std::thread::id{0};
        auto thread2 = std::thread{[&tid2, &tid22]() {
            auto& eventLoop = EventLoop::instance();
            auto eventReceiver = TestEventReceiver();
            eventLoop.postEvent(&eventReceiver, std::make_unique<Event>());
            eventLoop.processEvents();
            tid2 = eventReceiver.tid;
            tid22 = std::this_thread::get_id();
        }};
        if (thread2.joinable()) {
            thread2.join();
        } else {
            FAIL("Thread2 is not joinable");
        }

        WHEN("The threads are done")
        {
            THEN("The event recievers id and the thread id should be the same.")
            {
                REQUIRE(tid11 == tid1);
                REQUIRE(tid22 == tid2);
            }
        }
    }
}

SCENARIO("An Eventloop shall block the running thread until the quit event occurs")
{
    GIVEN("An EventLoop")
    {
        auto& eventloop = EventLoop::instance();
        WHEN("The quit event is posted to the EventLoop")
        {
            auto thread = std::thread([&eventloop] {
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
                eventloop.quit();
            });
            eventloop.exec();
            THEN("The EventLoop exit")
            {
                thread.join();
                REQUIRE(true);
            }
        }
    }
}

SCENARIO("An Eventloop shall be possible to check if a certian event is in the evenloop")
{

    GIVEN("An EventLoop")
    {
        auto& eventLoop = EventLoop::instance();
        auto eventReceiver = TestEventReceiver{};

        WHEN("A event is posted for a receiver")
        {
            eventLoop.postEvent(&eventReceiver, std::make_unique<Event>(Event::Type::Unknown));
            THEN("The check for the event is present shall be true")
            {
                REQUIRE(eventLoop.isEventQueued(&eventReceiver, Event::Type::Unknown));
            }
        }
    }
}

TEST_CASE("An Eventloop shall be deliver deferred connections in the correct thread")
{
    auto& eventLoop = EventLoop::instance();
    auto doneSignal = KDBindings::Signal<>{};
    auto tid = std::this_thread ::get_id();
    auto callerTid = std::thread::id{};
    std::ignore = doneSignal.connectDeferred(eventLoop.getConnectionEvaluator(), [&callerTid]() {
        callerTid = std::this_thread::get_id();
    });
    std::thread thread = std::thread{[&doneSignal] {
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
        doneSignal.emit();
    }};
    REQUIRE_COMPARE_WITH_TIMEOUT(callerTid, tid, std::chrono::milliseconds{10});
    thread.join();
}
