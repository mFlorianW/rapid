// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "FdNotifier.hpp"
#include "private/linux/FdNotifierImpl.hpp"
#include <spdlog/spdlog.h>

using namespace Rapid::System::Private::Linux;

namespace Rapid::System
{

FdNotifier::FdNotifier(int fd, FdNotifierType type)
    : mFd{fd}
    , mType{type}
{
    FdNotifierImpl::getInstance().registerNotifier(this, type);
}

FdNotifier::~FdNotifier()
{
    FdNotifierImpl::getInstance().unregisterNotifier(this);
}

int FdNotifier::getFd() const noexcept
{
    return mFd;
}

FdNotifierType FdNotifier::getType() const noexcept
{
    return mType;
}

bool FdNotifier::handleEvent(Event* event)
{
    if (event->getEventType() == Event::Type::Notifier) {
        notify.emit(mFd, mType);
        return true;
    }
    return false;
}

} // namespace Rapid::System
