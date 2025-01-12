// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "EventLoopIntegration.hpp"
#include <QAbstractEventDispatcher>
#include <spdlog/spdlog.h>
#include <system/EventLoop.hpp>
#include <unordered_map>

namespace Rapid::System::Qt::EventLoopIntegration
{

class EventLoopIntegration
{
public:
    EventLoopIntegration(QAbstractEventDispatcher* eventDispatcher)
    {
        assert(eventDispatcher != nullptr);
        QObject::connect(eventDispatcher, &QAbstractEventDispatcher::awake, eventDispatcher, [] {
            EventLoop::instance().processEvents();
        });

        QObject::connect(eventDispatcher, &QAbstractEventDispatcher::destroyed, eventDispatcher, [eventDispatcher]() {
            mIntegrations.erase(eventDispatcher);
        });

        std::ignore = EventLoop::instance().wakeUp.connect([eventDispatcher]() {
            eventDispatcher->wakeUp();
        });
    }

    static std::unordered_map<QAbstractEventDispatcher*, std::unique_ptr<EventLoopIntegration>> mIntegrations;

private:
};

std::unordered_map<QAbstractEventDispatcher*, std::unique_ptr<EventLoopIntegration>>
    EventLoopIntegration::mIntegrations;

bool makeEventLoopIntegration()
{
    auto eventDispatcher = QAbstractEventDispatcher::instance();
    if (eventDispatcher == nullptr) {
        SPDLOG_CRITICAL("The eventloop integration is only possible in threads with QEventLoop active. Integration has "
                        "to be happen after creating the Qt based eventloop.");
        return false;
    }
    EventLoopIntegration::mIntegrations.insert(
        {eventDispatcher, std::make_unique<EventLoopIntegration>(eventDispatcher)});
    return true;
}

} // namespace Rapid::System::Qt::EventLoopIntegration
