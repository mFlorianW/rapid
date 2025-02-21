// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_RESTACTIVESESSION_HPP
#define RAPID_WORKFLOW_RESTACTIVESESSION_HPP

#include <memory>
#include <workflow/IRestActiveSession.hpp>

namespace Rapid::Rest
{
class IRestClient;
class RestCall;
} // namespace Rapid::Rest

namespace Rapid::Workflow
{

/**
 * @copydoc Rapid::Workflow::IRestActiveSession
 */
class RestActiveSession : public IRestActiveSession
{
public:
    /**
     * @brief Creates an instance of  @ref RestActiveSession
     *
     * @details The class doesn't take the ownership of the passed client.
     *          So the rest client shall live as long as the @ref RestActiveSession instance.
     *
     * @param restclient The client that is used to send the REST requests.
     *                   The client must be fully configured that means the server address and port are set.
     */
    RestActiveSession(Rest::IRestClient* restClient);

    /**
     * @copydoc Rapid::Workflow::IRestActiveSession::downloadTrack
     */
    void updateTrackData() noexcept override;

    /**
     * @copydoc Rapid::Workflow::IRestActiveSession::updateLapInformation
     */
    void updateLapData() noexcept override;

protected:
    Rest::IRestClient* mRestClient{nullptr};

private:
    void onDownloadTrackFinished(Rest::RestCall* call) noexcept;
    void onUpdateLapInformationFinished(Rest::RestCall* call) noexcept;

    std::shared_ptr<Rest::RestCall> mPendingTrackCall;
    KDBindings::ScopedConnection mPendingTrackCallConnection;
    std::shared_ptr<Rest::RestCall> mPendingLapCall;
    KDBindings::ScopedConnection mPendingLapCallConnection;
};

} // namespace Rapid::Workflow

#endif //! RAPID_WORKFLOW_RESTACTIVESESSION_HPP
