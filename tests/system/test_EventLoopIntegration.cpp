// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QAbstractEventDispatcher>
#include <QSignalSpy>
#include <QTest>
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <system/EventHandler.hpp>
#include <system/EventLoop.hpp>
#include <system/Timer.hpp>
#include <system/qt/EventLoopIntegration.hpp>
#include <system/qt/RapidApplication.hpp>
#include <testhelper/qt/QtTestHelper.hpp>

using namespace Rapid::System;
using namespace Rapid::System::Qt;

struct TestEventHandler : public EventHandler
{
    MAKE_MOCK(handleEvent, auto(Event* e)->bool, override);
};

struct WorkerThread : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void threadFunc()
    {
        REQUIRE(EventLoopIntegration::makeEventLoopIntegration());
        auto timer = Rapid::System::Timer{};
        timer.setInterval(std::chrono::milliseconds{1});
        std::ignore = timer.timeout.connect([this] {
            timedout = true;
        });
        timer.start();
        REQUIRE(QTest::qWaitFor([this] {
            return timedout == true;
        }));
    }

public:
    bool timedout = false;
};

TEST_CASE("Event loop integration in Qt main loop")
{
    auto timedout = false;
    auto timer = Rapid::System::Timer{};
    timer.setInterval(std::chrono::milliseconds{1});
    std::ignore = timer.timeout.connect([&timedout] {
        timedout = true;
    });
    timer.start();
    REQUIRE(QTest::qWaitFor([&timedout] {
        return timedout == true;
    }));
}

TEST_CASE("Event loop integration in a Qt thread with Qt event loop")
{
    auto worker = WorkerThread();
    auto timer = Rapid::System::Timer{};
    auto thread = QThread();
    worker.moveToThread(&thread);
    QObject::connect(&thread, &QThread::started, &worker, &WorkerThread::threadFunc);
    thread.start();
    CHECK(QTest::qWaitFor([&worker] {
        return worker.timedout = true;
    }));
    thread.quit();
    thread.wait(5000);
    REQUIRE(worker.timedout);
}

QT_CATCH2_TEST_MAIN();

#include "test_EventLoopIntegration.moc"
