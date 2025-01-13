// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_SYSTEM_FDNOTIFIER_HPP
#define RAPID_SYSTEM_FDNOTIFIER_HPP

#include <cstdint>
#include <kdbindings/signal.h>
#include <system/EventHandler.hpp>

namespace Rapid::System
{

/**
 * The support notification types of the @ref FdNotifier
 */
enum FdNotifierType : std::uint8_t
{
    Read,
    Write
};

/**
 * @brief Give async notifications about changes on file descriptors
 *
 * @details The @ref FdNotifier make it's possible to observe any file descriptor activities without blocking the mainloop.
 *          The @ref FdNotifier support two notification types:
 *          - @ref FdNotifierType::Read
 *            This type signals when the observed file descriptor has data to read.
 *          - @ref FdNotifierType::Write
 *            This type signals when the observed file descriptor is able to write data.
 *          For observing one file descriptor for write and read it's necessary to create two instances.
 */
class FdNotifier : public System::EventHandler
{
public:
    /**
     * @brief Creates an instance of the @ref FdNotifier with @ref FdNotifierType type
     *
     * @details This constructor allows delay activation of the @ref FdNotifier.
     *          By setting the file descriptor with @ref FdNotifier::setFd activates the notification of the @ref FdNotifier.
     *
     * @param type The type of the file descriptor that shall be observed.
     */
    FdNotifier(FdNotifierType type);

    /**
     * Construct a @ref FdNotifier instance.
     * @param fd The file descriptor itself.
     * @param type The type of the file descriptor that shall be observed.
     */
    FdNotifier(int fd, FdNotifierType type);

    /**
     * Default destructor
     */
    ~FdNotifier() override;

    /**
     * Disabled copy constructor
     */
    FdNotifier(FdNotifier const&) = delete;

    /**
     * Disabled move constructor
     */
    FdNotifier& operator=(FdNotifier const&) = delete;

    /**
     * Disabled move constructor
     */
    FdNotifier(FdNotifier&&) noexcept = delete;

    /**
     * Disabled move operator
     */
    FdNotifier& operator=(FdNotifier&&) noexcept = delete;

    /**
     * @brief Gives the observed file descriptor
     */
    int getFd() const noexcept;

    /**
     * @brief Sets the file descriptor and activities the notification for the file descriptor.
     *
     * @details The notification of previous set file descriptors are disabled.
     *
     * @param fd The new file descriptor that shall be observed for changes.
     */
    void setFd(int fd);

    /**
     * @brief Gives the observed type of the file descriptor.
     */
    FdNotifierType getType() const noexcept;

    /**
     * @brief This signal is emitted when the system anounces activity on the file descriptor.
     *
     * @param fd The file descriptor itself
     * @param type The type of the file descriptor that is observed.
     */
    KDBindings::Signal<int, FdNotifierType> notify;

protected:
    bool handleEvent(Event* event) override;

private:
    static constexpr auto INVALID_SOCKET = int{-1};
    int mFd = INVALID_SOCKET;
    FdNotifierType mType;
};

} // namespace Rapid::System

#endif // !RAPID_SYSTEM_FDNOTIFIER_HPP
