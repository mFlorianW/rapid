// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_LOCALSESSIONMANAGEMENT
#define RAPID_WORKFLOW_LOCALSESSIONMANAGEMENT

#include <workflow/ISessionManagement.hpp>

namespace Rapid::Workflow
{

/**
 * @brief Provides the session management for the local device
 */
class LocalSessionManagement : public ISessionManagement
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

    /** @cond Doxygen_Suppress */
    ~LocalSessionManagement() override;
    LocalSessionManagement(LocalSessionManagement const&) = delete;
    LocalSessionManagement& operator=(LocalSessionManagement const&) = delete;
    LocalSessionManagement(LocalSessionManagement const&&) noexcept = delete;
    LocalSessionManagement& operator=(LocalSessionManagement&&) noexcept = delete;
    /** @endcond Doxygen_Suppress */

    /**
     * @copydoc Rapid::Workflow::ISessionManagement::getSession
     */
    [[nodiscard]] std::shared_ptr<Storage::GetSessionResult> getSession(
        Common::SessionMetaData const& metadata) override;

protected:
    Storage::ISessionDatabase* mDb;
};

}; // namespace Rapid::Workflow

#endif // !RAPID_WORKFLOW_LOCALSESSIONMANAGEMENT
