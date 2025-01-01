// SPDX-FileCopyrightText: 2024 - 2025 All contributors
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
 * This interface contains the functionality to manage the Sessions of a laptimer device via the REST interface.
 * The session can be downloaded in a index based manner.
 * The first session index starts at 0 and ends at sessionCount - 1.
 */
class IRestSessionManagementWorkflow
{
public:
    virtual ~IRestSessionManagementWorkflow() noexcept = default;

    /**
     * Deleted copy constructor
     */
    IRestSessionManagementWorkflow(IRestSessionManagementWorkflow const&) = delete;

    /**
     * Deleted copy operator
     */
    IRestSessionManagementWorkflow& operator=(IRestSessionManagementWorkflow const&) = delete;

    /**
     * Deleted move constructor
     */
    IRestSessionManagementWorkflow(IRestSessionManagementWorkflow&&) = delete;

    /**
     * Deleted move operator
     */
    IRestSessionManagementWorkflow& operator=(IRestSessionManagementWorkflow&&) = delete;

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
     * @brief Gives the session data for the passed index.
     *
     * @details The return is only valid when the session metadata previously was downloaded.
     *          The download is started with @ref downloadSessionMetadata.
     *
     * @param index The index of the session.
     * @return The session data for the passed index.
     */
    virtual std::optional<Common::SessionMetaData> getSessionMetadata(std::size_t index) const noexcept = 0;

    /**
     * Downloads a specific session of the device.
     */
    virtual void downloadSession(std::size_t index) noexcept = 0;

    /**
     * @brief Downloads the session metadata from the device
     *
     * @details The download result is reported with signal @ref sessionMetadataDownloadFinished
     *          The actual metadata then can received the with @ref getSessionMetadatafunction.
     *
     * @param index The index of the session for which the session meta data shall be downloaded.
     */
    virtual void downloadSessionMetadata(std::size_t index) noexcept = 0;

    /**
     * @brief Downloads all session metadata from the device
     *
     * @details The downloaded metadata is reported with the signal @ref sessionMetadataDownloadFinished
     */
    virtual void downloadAllSessionMetadata() noexcept = 0;

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

    /**
     * @brief This signal is emitted when a session metadata download is finished.
     * @param std::size_t The index of the session which is finished.
     * @param DownloadResult The result of the session download.
     */
    KDBindings::Signal<std::size_t, DownloadResult> sessionMetadataDownloadFinished;

protected:
    IRestSessionManagementWorkflow() noexcept = default;
};
} // namespace Rapid::Workflow
