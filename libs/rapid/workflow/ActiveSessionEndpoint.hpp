// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_ACTIVESESSIONENDPOINT_HPP
#define RAPID_WORKFLOW_ACTIVESESSIONENDPOINT_HPP

#include <rest/IRestRequestHandler.hpp>
#include <workflow/IActiveSessionWorkflow.hpp>

namespace Rapid::Workflow
{

/**
 * @brief The @ref ActiveSessionEndpoint provides the information of the current active session.
 *
 * @details The @ref ActiveSessionEndpoint provides two endpoints:
 *          - /activeSession/track
 *            Provides the used track for the session
 *          - /activeSession/lap
 *            Provides the current lap information
 *            - Lap count
 *            - Last lap time
 *            - Last sector time
 *            - Current lap time
 *            - Current sector time
 */
class ActiveSessionEndpoint : public Rest::IRestRequestHandler
{
public:
    /**
     * @brief Creates an instance of the @ref ActiveSessionEndpoint
     * @param activeSessionWorkflow The active session to gather the information from
     */
    explicit ActiveSessionEndpoint(IActiveSessionWorkflow* activeSessionWorkflow);

    /**
     * Default destructor
     */
    ~ActiveSessionEndpoint() override;

    /**
     * Disalbed copy constructor
     */
    ActiveSessionEndpoint(ActiveSessionEndpoint const&) = delete;

    /**
     * Disalbed copy operator
     */
    ActiveSessionEndpoint& operator=(ActiveSessionEndpoint const&) = delete;

    /**
     * Disalbed move constructor
     */
    ActiveSessionEndpoint(ActiveSessionEndpoint&&) noexcept = delete;

    /**
     * Disalbed move operator
     */
    ActiveSessionEndpoint& operator=(ActiveSessionEndpoint&&) noexcept = delete;

    /**
     * @copydoc IRestRequestHandler::handleRestRequest
     */
    void handleRestRequest(Rest::RestRequest& request) noexcept override;

private:
    void handleTrackRequest(Rest::RestRequest& request);
    void handleLapRequest(Rest::RestRequest& request);
    bool isValidPath(Rest::Path const& path) const noexcept;

    IActiveSessionWorkflow* mActiveSessionWorkflow = nullptr;
};

} // namespace Rapid::Workflow

#endif // !RAPID_WORKFLOW_ACTIVESESSIONENDPOINT_HPP
