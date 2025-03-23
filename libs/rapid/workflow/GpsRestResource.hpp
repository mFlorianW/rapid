// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_IGPSRESTRESOURCE_HPP
#define RAPID_WORKFLOW_IGPSRESTRESOURCE_HPP

#include <positioning/IGPSInformationProvider.hpp>
#include <positioning/IGpsPositionProvider.hpp>
#include <rest/IRestRequestHandler.hpp>

namespace Rapid::Workflow
{

/**
 * @brief The @ref Rapid::Workflow::GpsRestResource provides the GPS resources via REST
 *
 * @details The following resources are accessable when the @ref Rapid::Workflow::GpsRestResource is registered in a @ref Rapid::Rest::IRestServer:
 *          - /gps/fixmode
 *            Provides the current status of fix mode property of the GPS modul
 *          - /gps/satellites
 *            Provides the number of sattellites used the GPS modul
 *          - /gps/position
 *            Provides the last position of the GPS modul
 */
class GpsRestResource : public Rest::IRestRequestHandler
{
public:
    /**
     * @brief Creates an instance of the @ref Rapid::Workflow::GpsRestResource
     *
     * @details This class doesn't take any ownershipt of the provided pointer.
     *          The caller must guarantee that the life dependencies have the same life time as the @ref Rapid::Workflow::GpsRestResource.
     *
     * @param gpsInfoProvider The source of the GPS information
     * @param gpsPosProvider The source of the GPS position
     */
    GpsRestResource(Positioning::IGpsInformationProvider* gpsInfoProvider,
                    Positioning::IGpsPositionProvider* gpsPosProvider);

    /**
     * @copydoc Rapid::Rest::IRestRequestHandler::handleRestRequest
     */
    void handleRestRequest(Rest::RestRequest& request) noexcept override;

private:
    Positioning::IGpsInformationProvider* mGpsInfoProvider{nullptr};
    Positioning::IGpsPositionProvider* mGpsPosProvider{nullptr};
};

} // namespace Rapid::Workflow

#endif // !RAPID_WORKFLOW_IGPSRESTRESOURCE_HPP
