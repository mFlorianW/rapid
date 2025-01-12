// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "FdNotifierImpl.hpp"
#include "system/EventLoop.hpp"
#include <memory>
#include <ranges>
#include <spdlog/spdlog.h>
#include <sys/epoll.h>

namespace Rapid::System::Private::Linux
{

FdNotifierImpl::FdNotifierImpl()
    : mNotifiers{std::make_unique<std::unordered_map<int, EventNode>>()}
    , mEpollFd{epoll_create1(EPOLL_CLOEXEC)}
{
    if (mEpollFd < -1) {
        SPDLOG_ERROR("Failed to create epoll instance. FdNotifier will not work. Error: {}", strerror(errno));
    }
}

FdNotifierImpl::~FdNotifierImpl()
{
    if (mPollThread != nullptr) {
        stopPolling();
    }
};

FdNotifierImpl& FdNotifierImpl::getInstance()
{
    static auto impl = FdNotifierImpl{};
    return impl;
}

void FdNotifierImpl::registerNotifier(Rapid::System::FdNotifier* fdNotifier, FdNotifierType type)
{
    auto guard = std::lock_guard<std::mutex>{mMutex};
    auto& entryNode = (*mNotifiers)[fdNotifier->getFd()];
    if (isRegistered(entryNode, fdNotifier->getType())) {
        SPDLOG_WARN("FdNotifier {} for type {} already registered. Ignoring registration.");
        return;
    }
    auto const operation = entryNode.isEmpty() ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    updateEntry(fdNotifier, entryNode);

    auto eventData = epoll_data_t{};
    eventData.fd = fdNotifier->getFd();
    auto const events = getRegisterEvents(entryNode);
    auto event = epoll_event{.events = events, .data = eventData};
    auto result = epoll_ctl(mEpollFd, operation, event.data.fd, &event);
    if (result < 0) {
        SPDLOG_ERROR("Failed to regiester file descriptor {}. Error: {}", fdNotifier->getFd(), strerror(errno));
        mNotifiers->erase(fdNotifier->getFd());
        return;
    }

    if (not mRunning and mNotifiers->size() == 1 and mPollThread == nullptr) {
        startPolling();
    }
    SPDLOG_DEBUG("Register fd notifier {} with type {}", fdNotifier->getFd(), fdNotifier->getType());
}

void FdNotifierImpl::unregisterNotifier(Rapid::System::FdNotifier* fdNotifier)
{
    auto& entryNode = (*mNotifiers)[fdNotifier->getFd()];
    if (entryNode.isEmpty()) {
        return;
    }

    auto eventData = epoll_data_t{};
    eventData.fd = fdNotifier->getFd();
    auto const operation = getUnregisterOperation(*fdNotifier);
    auto const events = getUnregisterEvents(entryNode, fdNotifier->getType());
    auto event = epoll_event{.events = events, .data = eventData};
    auto result = epoll_ctl(mEpollFd, operation, event.data.fd, &event);
    if (result < 0) {
        SPDLOG_ERROR("Failed to unregiester file descriptor {}. Error: {}", fdNotifier->getFd(), strerror(errno));
        return;
    }

    if (operation == EPOLL_CTL_DEL) {
        auto guard = std::lock_guard<std::mutex>{mMutex};
        mNotifiers->erase(fdNotifier->getFd());
        if (mRunning and mNotifiers->size() == 0) {
            stopPolling();
        }
    } else {
        if (fdNotifier->getType() == FdNotifierType::Read) {
            (*mNotifiers)[fdNotifier->getFd()].readNotifitier = nullptr;
        } else {
            (*mNotifiers)[fdNotifier->getFd()].writeNotifier = nullptr;
        }
    }
    SPDLOG_DEBUG("Unregister fd notifier {} with type {}", fdNotifier->getFd(), fdNotifier->getType());
}

void FdNotifierImpl::polling()
{
    constexpr std::size_t maxEvents = 16;
    constexpr std::uint16_t timeout = 500;
    auto events = std::array<epoll_event, maxEvents>{};
    while (mRunning) {
        auto eventCount = epoll_wait(mEpollFd, events.data(), events.size(), timeout);
        if (eventCount < 0 and errno != EINTR) {
            SPDLOG_ERROR("EPOLL_WAIT error occurs. Stopping event processing. Error: {} {}", errno, strerror(errno));
            break;
        } else if (eventCount > 0) {
            for (auto const& eventIndex : std::views::iota(0, eventCount)) {
                auto event = events.at(eventIndex);
                auto fd = event.data.fd;
                auto guard = std::lock_guard<std::mutex>(mMutex);
                FdNotifier* fdNotifier = nullptr;
                if (event.events & EPOLLIN and mNotifiers->count(fd) > 0) {
                    fdNotifier = mNotifiers->at(event.data.fd).readNotifitier;
                } else if (event.events & EPOLLOUT and mNotifiers->count(fd) > 0) {
                    fdNotifier = mNotifiers->at(event.data.fd).writeNotifier;
                }
                if (fdNotifier != nullptr) {
                    EventLoop::postEvent(fdNotifier, std::make_unique<Event>(Event::Type::Notifier));
                    SPDLOG_DEBUG("Post nofitier event for fd notifier {:X} , Type {}",
                                 fdNotifier->getFd(),
                                 static_cast<int>(fdNotifier->getType()));
                } else {
                    SPDLOG_ERROR("EPOLL called with an unregistered file descriptor");
                }
            }
        }
    }
}

void FdNotifierImpl::startPolling()
{
    mPollThread = std::make_unique<std::thread>([this] {
        mRunning = true;
        polling();
    });
}

void FdNotifierImpl::stopPolling()
{
    mRunning = false;
    if (mPollThread->joinable()) {
        mPollThread->join();
    }
    mPollThread = nullptr;
}

bool FdNotifierImpl::isRegistered(EventNode const& node, FdNotifierType type)
{
    if (type == FdNotifierType::Read) {
        return node.hasReadNotifier();
    } else {
        return node.hasWriteNotifier();
    }
}

std::uint32_t FdNotifierImpl::getRegisterEvents(EventNode const& node) const noexcept
{
    auto events = std::uint32_t{0};
    if (node.hasReadNotifier()) {
        events |= EPOLLIN;
    }
    if (node.hasWriteNotifier()) {
        events |= EPOLLOUT;
    }
    return events | EPOLLET;
}

std::uint32_t FdNotifierImpl::getUnregisterEvents(EventNode const& node, FdNotifierType type) const noexcept
{
    auto events = std::uint32_t{0};
    if (type == FdNotifierType::Read && node.hasWriteNotifier()) {
        events = EPOLLOUT | EPOLLET;
    } else if (type == FdNotifierType::Write && node.hasReadNotifier()) {
        events = EPOLLIN | EPOLLET;
    }
    return events;
}

std::uint8_t FdNotifierImpl::getRegisterOperation(FdNotifier const& fdNotifier) noexcept
{
    return mNotifiers->count(fdNotifier.getFd()) > 0 ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
}

std::uint8_t FdNotifierImpl::getUnregisterOperation(FdNotifier const& fdNotifier) noexcept
{
    if (mNotifiers->count(fdNotifier.getFd()) == 0) {
        SPDLOG_ERROR(
            "Failed to define unregister operation for EPOLL. Error: Function called with invalid file descriper {}",
            fdNotifier.getFd());
        return EPOLL_CTL_DEL;
    }
    auto isDeletetion = mNotifiers->at(fdNotifier.getFd()).deleteOrModify(fdNotifier.getType());
    return isDeletetion == true ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
}

void FdNotifierImpl::updateEntry(FdNotifier* fdNotifier, EventNode& node)
{
    if (fdNotifier->getType() == Read) {
        node.readNotifitier = fdNotifier;
    } else {
        node.writeNotifier = fdNotifier;
    }
}

} // namespace Rapid::System::Private::Linux
