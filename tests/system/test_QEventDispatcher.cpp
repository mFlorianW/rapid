// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QEventDispatcher.hpp>
#include <QTest>
#include <Timer.hpp>

constexpr auto timeout = std::chrono::milliseconds{100}.count();

class QEventDispatcherTest : public QObject
{
    Q_OBJECT
public:
    QEventDispatcherTest() = default;
    ~QEventDispatcherTest() override = default;
    Q_DISABLE_COPY_MOVE(QEventDispatcherTest);
private Q_SLOTS:
    void timerEventsAreWorking()
    {
        auto timedout = false;
        auto timer = Rapid::System::Timer{};
        timer.setInterval(std::chrono::milliseconds{1});
        timer.timeout.connect([&timedout] {
            timedout = true;
        });
        timer.start();
        QTRY_COMPARE_WITH_TIMEOUT(timedout, true, timeout);
    }
};

int main(int argc, char** argv)
{
    auto disp = new Rapid::System::Qt::QEventDispatcher{}; // NOLINT(cppcoreguidelines-owning-memory)
    QCoreApplication::setEventDispatcher(disp);
    QCoreApplication app(argc, argv);

    // Initialize and run the test object
    QEventDispatcherTest testObj;

    // Use QTest::qExec to run the tests manually
    return QTest::qExec(&testObj, argc, argv);
}

#include "test_QEventDispatcher.moc"
