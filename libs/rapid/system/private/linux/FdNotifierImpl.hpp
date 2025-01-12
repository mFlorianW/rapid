// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_SYSTEM_PRIVATE_FDNOTIFIERIMPL_HPP
#define RAPID_SYSTEM_PRIVATE_FDNOTIFIERIMPL_HPP

#include "system/FdNotifier.hpp"
#include <thread>

namespace Rapid::System::Private::Linux
{

class FdNotifierImpl final
{
public:
    ~FdNotifierImpl();
    FdNotifierImpl(FdNotifierImpl const&) = delete;
    FdNotifierImpl& operator=(FdNotifierImpl const&) = delete;
    FdNotifierImpl(FdNotifierImpl const&&) noexcept = delete;
    FdNotifierImpl& operator=(FdNotifierImpl&&) noexcept = delete;

    static FdNotifierImpl& getInstance();

    void registerNotifier(Rapid::System::FdNotifier* fdNotifier, FdNotifierType type);
    void unregisterNotifier(Rapid::System::FdNotifier* fdNotifier);

protected:
    FdNotifierImpl();

private:
    struct EventNode
    {
        FdNotifier* readNotifitier{nullptr};
        FdNotifier* writeNotifier{nullptr};

        bool deleteOrModify(FdNotifierType type)
        {
            switch (type) {
            case FdNotifierType::Read:
                return not hasWriteNotifier();
            case FdNotifierType::Write:
                return not hasReadNotifier();
            }
            return true;
        }

        [[nodiscard]] bool isEmpty() const
        {
            return not hasReadNotifier() and not hasWriteNotifier();
        }

        [[nodiscard]] bool hasReadNotifier() const noexcept
        {
            return readNotifitier != nullptr;
        }

        [[nodiscard]] bool hasWriteNotifier() const noexcept
        {
            return writeNotifier != nullptr;
        }
    };

    void polling();
    void startPolling();
    void stopPolling();

    bool isRegistered(EventNode const& node, FdNotifierType type);
    std::uint32_t getRegisterEvents(EventNode const& node) const noexcept;
    std::uint32_t getUnregisterEvents(EventNode const& node, FdNotifierType type) const noexcept;
    std::uint8_t getRegisterOperation(FdNotifier const& fdNotifier) noexcept;
    std::uint8_t getUnregisterOperation(FdNotifier const& FdNotifier) noexcept;
    void updateEntry(FdNotifier* fdNotifier, EventNode& node);

    std::mutex mMutex;
    std::unique_ptr<std::unordered_map<int, EventNode>> mNotifiers;
    std::unique_ptr<std::thread> mPollThread;
    std::atomic<bool> mRunning = false;
    int mEpollFd{-1};
};

} // namespace Rapid::System::Private::Linux

#endif // !RAPID_SYSTEM_PRIVATE_FDNOTIFIERIMPL_HPP
