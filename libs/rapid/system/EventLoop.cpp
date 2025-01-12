// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "EventLoop.hpp"
#include <condition_variable>
#include <deque>
#include <kdbindings/connection_evaluator.h>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <sstream>
#include <unordered_map>

namespace Rapid::System
{
class EventQueue;

class ConnectionEvaluator : public KDBindings::ConnectionEvaluator
{
public:
    ConnectionEvaluator(EventQueue& eventQueue)
        : mEventQueue(eventQueue)
    {
    }
    ~ConnectionEvaluator() override = default;
    ConnectionEvaluator& operator=(ConnectionEvaluator const&) = delete;
    ConnectionEvaluator(ConnectionEvaluator const&) = delete;
    ConnectionEvaluator& operator=(ConnectionEvaluator&&) noexcept = delete;
    ConnectionEvaluator(ConnectionEvaluator&&) noexcept = delete;

protected:
    void onInvocationAdded() override;

private:
    EventQueue& mEventQueue;
};

namespace
{

inline std::string getThreadIdAsString(std::thread::id const& id)
{
    auto tid = std::stringstream{};
    tid << std::hex << id;
    return tid.str();
}
} // namespace

class EventQueue
{
public:
    EventQueue() = default;

    static EventQueue& getInstance(std::thread::id const& tid)
    {
        static std::mutex mutex;
        static std::unordered_map<std::thread::id, std::unique_ptr<EventQueue>> eventQueues;
        if (eventQueues.count(tid) == 0) {
            std::lock_guard<std::mutex> guard{mutex};
            eventQueues.emplace(tid, std::make_unique<EventQueue>());
        }
        return *eventQueues[tid];
    }

    void postEvent(EventHandler* receiver, std::unique_ptr<Event> event, std::thread::id const& tid)
    {

        if (tid != receiver->getThreadId()) {
            spdlog::error(
                "The event receiver has a different thread affinity. Event posted to wrong event loop instance.");
            return;
        }

        {
            std::lock_guard<std::mutex> guard{mMutex};
            mEventQueue.push_back(EventQueueEntry{.receiver = receiver, .event = std::move(event)});
        }
        mBlocker.notify_all();
    }

    void processEvents()
    {
        auto iter = mEventQueue.begin();
        mConnectionEvaluator->evaluateDeferredConnections();
        while (iter != mEventQueue.end()) {
            auto event = std::move(iter->event);
            auto* receiver = iter->receiver;
            iter = mEventQueue.erase(iter);
            receiver->handleEvent(event.get());
            mIsWoken = false;
        }
    }

    void exec()
    {
        mRunning = true;
        while (mRunning) {
            if (not mRunning) {
                break;
            }
            std::unique_lock uLock{mMutex};
            mBlocker.wait(uLock);
            processEvents();
        }
    }

    void clearQueue()
    {
        mEventQueue.clear();
    }

    void stopEventLoop()
    {
        {
            std::lock_guard<std::mutex> guard{mMutex};
            mRunning = false;
        }
        mBlocker.notify_one();
    }

    bool isEventQueued(EventHandler* receiver, Event::Type type) const noexcept
    {
        for (auto const& entry : mEventQueue) {
            if (entry.receiver == receiver and entry.event->getEventType() == type) {
                return true;
            }
        }
        return false;
    }

    void clearEvents(EventHandler* eventHandler)
    {
        auto iter = mEventQueue.begin();
        while (iter != mEventQueue.end()) {
            if (iter->receiver == eventHandler) {
                std::lock_guard<std::mutex> guard{mMutex};
                iter = mEventQueue.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    std::shared_ptr<KDBindings::ConnectionEvaluator> getConnectEvaluator()
    {
        return mConnectionEvaluator;
    }

    bool isRunning()
    {
        return mRunning;
    }

    void unblock()
    {
        mBlocker.notify_all();
    }

    bool isWoken()
    {
        return mIsWoken;
    }

    void setWoken(bool woken)
    {
        mIsWoken = woken;
    }

    KDBindings::Signal<> wakeUp;

private:
    struct EventQueueEntry
    {
        EventHandler* receiver;
        std::unique_ptr<Event> event;
    };
    std::deque<EventQueueEntry> mEventQueue;
    mutable std::mutex mMutex;
    std::condition_variable mBlocker;
    std::atomic<bool> mRunning = false;
    std::atomic<bool> mIsWoken = false;
    std::shared_ptr<KDBindings::ConnectionEvaluator> mConnectionEvaluator =
        std::make_shared<ConnectionEvaluator>(*this);
};

void ConnectionEvaluator::onInvocationAdded()
{
    if (mEventQueue.isRunning()) {
        mEventQueue.unblock();
    } else {
        mEventQueue.wakeUp.emit();
    }
}

EventLoop::EventLoop(EventQueue& queue)
    : mOwningThread{std::this_thread::get_id()}
    , mEventQueue{queue}
{
    std::ignore = queue.wakeUp.connect([this] {
        wakeUp.emit();
    });
}

EventLoop::~EventLoop() = default;

EventLoop& EventLoop::instance() noexcept
{
    static std::mutex mutex;
    auto const id = std::this_thread::get_id();
    if (mEventLoops.count(id) == 0) {
        std::lock_guard<std::mutex> guard{mutex};
        mEventLoops.emplace(
            id,
            std::unique_ptr<Rapid::System::EventLoop>(new (std::nothrow) EventLoop(EventQueue::getInstance(id))));
    }
    return *mEventLoops[id];
}

std::unordered_map<std::thread::id, std::unique_ptr<EventLoop>> EventLoop::mEventLoops;

void EventLoop::postEvent(EventHandler* receiver, std::unique_ptr<Event> event)
{
    auto& queue = EventQueue::getInstance(receiver->getThreadId());
    queue.postEvent(receiver, std::move(event), receiver->getThreadId());
    // SPDLOG_INFO("sdfsdfsdfsdfsdfsdf {}", queue.isRunning());
    if (mEventLoops.count(receiver->getThreadId()) > 0 and not queue.isRunning() and not queue.isWoken()) {
        queue.setWoken(true);
        mEventLoops[receiver->getThreadId()]->wakeUp.emit();
    }
}

bool EventLoop::isEventQueued(EventHandler* receiver, Event::Type type) const noexcept
{
    return EventQueue::getInstance(mOwningThread).isEventQueued(receiver, type);
}

void EventLoop::processEvents()
{
    if (std::this_thread::get_id() != mOwningThread) {
        spdlog::error("Events can only be processed from the EventLoop owning thread.");
        spdlog::error("Owning thread: {}  Calling thread: {}",
                      getThreadIdAsString(mOwningThread),
                      getThreadIdAsString(std::this_thread::get_id()));
        return;
    }
    EventQueue::getInstance(mOwningThread).processEvents();
}

void EventLoop::exec()
{
    if (std::this_thread::get_id() != mOwningThread) {
        spdlog::error("Exec can only be called from the Eventloop owning thread.");
        spdlog::error("Owning thread: {}  Calling thread: {}",
                      getThreadIdAsString(mOwningThread),
                      getThreadIdAsString(std::this_thread::get_id()));
    }
    EventQueue::getInstance(mOwningThread).exec();
}

void EventLoop::quit() noexcept
{
    EventQueue::getInstance(mOwningThread).stopEventLoop();
}

void EventLoop::clearEvents(EventHandler* eventHandler) noexcept
{
    EventQueue::getInstance(eventHandler->getThreadId()).clearEvents(eventHandler);
}

std::shared_ptr<KDBindings::ConnectionEvaluator> EventLoop::getConnectionEvaluator()
{
    auto const tid = std::this_thread::get_id();
    if (mEventLoops.count(tid) == 0) {
        SPDLOG_ERROR("ConnectionEvaluator request for thread {} without event loop", getThreadIdAsString(tid));
        return EventLoop::instance().getConnectionEvaluator();
    }
    return mEventLoops[tid]->mEventQueue.getConnectEvaluator();
}

} // namespace Rapid::System
