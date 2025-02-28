// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_LOCALSESSIONMANAGEMENT
#define RAPID_WORKFLOW_LOCALSESSIONMANAGEMENT

#include <storage/ISessionDatabase.hpp>

namespace Rapid::Workflow
{

/**
 * @brief Provides the session management for the local device
 */
class LocalSessionManagement
{
public:
    /**
     * @brief Creates an instance of the @ref Rapid::Workflow::LocalSessionManagement class
     *
     * @param db A pointer to a @ref Rapid::Storage::ISessionDatabase instance.
     *           The LocalSessionManagement doesn't take the ownership.
     *           So the caller must guarantee that the instance has the same lifetime as the LocalSessionManagement
     */
    LocalSessionManagement(Storage::ISessionDatabase* db);

    /**
     * @brief Gives the @ref Rapid::Common::Session for the passed @ref Rapid::Common::SessionMetaData.
     *
     * @details This is an asynchronous operation so the returned @Rapid::Storage::GetSessionResult instance notifies when finished.
     *          But the caller should check immediatly the status of the result if an error happens no notification is emitted.
     *
     * @return The result object for the asynchronous operation.
     */
    std::shared_ptr<Storage::GetSessionResult> getSession(Common::SessionMetaData const& metadata);

private:
    Storage::ISessionDatabase* mDb;
};

}; // namespace Rapid::Workflow

#endif // !RAPID_WORKFLOW_LOCALSESSIONMANAGEMENT
