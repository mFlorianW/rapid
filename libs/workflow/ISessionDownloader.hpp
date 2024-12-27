// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "common/SessionData.hpp"
#include <kdbindings/signal.h>

namespace Rapid::Workflow
{

enum class DownloadResult
{
    Ok,
    Error
};

/**
 * This interface contains the functionality to download the Sessions from a laptimer
 * The session can be downloaded in a index based manner. The first session index starts at 0 and ends at sessionCount
 * - 1.
 */
class ISessionDownloader
{
public:
    virtual ~ISessionDownloader() noexcept = default;

    /**
     * Deleted copy constructor
     */
    ISessionDownloader(ISessionDownloader const&) = delete;

    /**
     * Deleted copy operator
     */
    ISessionDownloader& operator=(ISessionDownloader const&) = delete;

    /**
     * Deleted move constructor
     */
    ISessionDownloader(ISessionDownloader&&) = delete;

    /**
     * Deleted move operator
     */
    ISessionDownloader& operator=(ISessionDownloader&&) = delete;

    /**
     * Gives the number of stored sessions of the laptimer. The return value is only valid after calling
     * fetchSessionCount and the signal sessionCountFetched was emitted.
     * @return The amount of session that are stored on the device.
     */
    virtual std::size_t getSessionCount() const noexcept = 0;

    /**
     * Fetches the session count of a laptimer device.
     * Shall emit the signal sessionCountFetched when finshed.
     */
    virtual void fetchSessionCount() noexcept = 0;

    /**
     * Gives the session data for the passed index. The return is only valid when the session was previously downloaded
     * with downloaded and the signal sessionDownloadFinshed.
     * @param index The index of the session.
     * @return The session data for the passed index.
     */
    virtual std::optional<Common::SessionData> getSession(std::size_t index) const noexcept = 0;

    /**
     * Downloads a specific session of the device.
     */
    virtual void downloadSession(std::size_t index) noexcept = 0;

    /**
     * This signal is emitted when the fetchSessionCount operation finshed.
     * @param DownloadResult The result of the call.
     */
    KDBindings::Signal<DownloadResult> sessionCountFetched;

    /**
     * This signal is emitted when a session download is finished.
     * @param std::size_t The index of the session which is finished.
     * @param DownloadResult The result of the session download.
     */
    KDBindings::Signal<std::size_t, DownloadResult> sessionDownloadFinshed;

protected:
    ISessionDownloader() noexcept = default;
};
} // namespace Rapid::Workflow
