// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_GPSRESTRESOURCEREADER_HPP
#define RAPID_WORKFLOW_GPSRESTRESOURCEREADER_HPP

#include <positioning/IGPSInformationProvider.hpp>
#include <rest/IRestClient.hpp>
#include <system/AsyncResult.hpp>

namespace Rapid::Workflow
{

/**
 * Alias for the fix mode REST request result
 */
using FixModeResult = System::AsyncResultWithValue<Positioning::GpsFixMode>;

/**
 * Alias for the satellites result
 */
using SatellitesResult = System::AsyncResultWithValue<std::uint8_t>;

/**
 * @brief Reads data from the @ref Rapid::Workflow::GpsRestResource
 */
class GpsRestResourceReader
{
public:
    /**
     * @brief Creates an instance of the @ref Rapid::Workflow::GpsRestResourceReader
     *
     * @param restClient The REST client that shall be used to communicate with the device.
     *                   The @ref Rapid::Workflow::GpsRestResourceReader doesnt't take any ownership of the IRestClient.
     *                   That means the IRestClient must have the same life time as this instance.
     */
    GpsRestResourceReader(Rest::IRestClient* restClient);

    /** @cond Doxygen_Suppress */
    ~GpsRestResourceReader();
    GpsRestResourceReader(GpsRestResourceReader const&) = delete;
    GpsRestResourceReader& operator=(GpsRestResourceReader const&) = delete;
    GpsRestResourceReader(GpsRestResourceReader&&) noexcept = default;
    GpsRestResourceReader& operator=(GpsRestResourceReader&&) noexcept = default;
    /** @endcond */

    /**
     * @brief Fetches the fix mode from the laptimer device.
     *
     * @return FixModeResult The result of the async operation.
     */
    std::shared_ptr<FixModeResult> getFixMode() noexcept;

    /**
     *
     * @brief Fetches the amount of used satellites from the laptimer device.
     *
     * @return SatellitesResult The result of the async operation.
     */
    std::shared_ptr<SatellitesResult> getSatellites() noexcept;

private:
    void handleFixModeRequestFinished(Rest::RestCall* call);
    void handleSatellitesRequestFinished(Rest::RestCall* call);

    Rest::IRestClient* mRestClient;

    template <typename RequestResult>
    struct RequestEntry
    {
        std::shared_ptr<RequestResult> result;
        std::shared_ptr<Rest::RestCall> call;
        KDBindings::ScopedConnection connection;
    };

    std::unordered_map<Rest::RestCall*, RequestEntry<FixModeResult>> mFixModeRequests;
    std::unordered_map<Rest::RestCall*, RequestEntry<SatellitesResult>> mSatellitesRequests;
};

} // namespace Rapid::Workflow

#endif // !RAPID_WORKFLOW_GPSRESTRESOURCEREADER_HPP
