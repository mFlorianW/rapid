// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_ISESSIONMANAGEMENT_HPP
#define RAPID_WORKFLOW_ISESSIONMANAGEMENT_HPP

#include <storage/ISessionDatabase.hpp>

namespace Rapid::Workflow
{

/**
 * @brief Defines the interface for managing the @ref Rapid::Common::SessionMetaData and @ref Rapid::Common::SessionData.
 */
class ISessionManagement
{
public:
    /** @cond Doxygen_Suppress */
    virtual ~ISessionManagement() = default;
    ISessionManagement(ISessionManagement const&) = delete;
    ISessionManagement& operator=(ISessionManagement const&) = delete;
    ISessionManagement(ISessionManagement&&) = delete;
    ISessionManagement& operator=(ISessionManagement&&) noexcept = delete;
    /** @endcond Doxygen_Suppress */

    /**
     * @brief Gives the @ref Rapid::Common::Session for the passed @ref Rapid::Common::SessionMetaData.
     *
     * @details This is an asynchronous operation so the returned @Rapid::Storage::GetSessionResult instance notifies when finished.
     *          But the caller should check immediatly the status of the result if an error happens no notification is emitted.
     *
     * @return The result object for the asynchronous operation.
     */
    [[nodiscard]] virtual std::shared_ptr<Storage::GetSessionResult> getSession(
        Common::SessionMetaData const& metadata) = 0;

protected:
    ISessionManagement() = default;
};

} // namespace Rapid::Workflow

#endif //! RAPID_WORKFLOW_ISESSIONMANAGEMENT_HPP
