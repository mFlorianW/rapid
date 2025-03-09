// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_POSITIONING_GPSDPOSITIONINFORMATIONPROVIDER_HPP
#define RAPID_POSITIONING_GPSDPOSITIONINFORMATIONPROVIDER_HPP

#include <positioning/IGPSInformationProvider.hpp>
#include <positioning/IGpsPositionProvider.hpp>
#include <system/FdNotifier.hpp>

namespace Rapid::Positioning
{

namespace Private
{
/** Forward declaration for private helper class */
class GpsdProvider;
} // namespace Private

/**
 * @breif An implementaion of the Rapid::Positioning::IGPSInformationProvider and Rapid::Positioning::IGPSInformationProvider interface.
 */
class GpsdPositionInformationProvider : public IGpsPositionProvider, IGpsInformationProvider
{
public:
    /**
     * @brief Creates an instace of the GpsdPositionInformationProvider
     *
     * @details The constructor set up an FIFO for the communication with GPSD.
     */
    GpsdPositionInformationProvider();

    /**@cond Doxygen_Suppress */
    ~GpsdPositionInformationProvider() override;
    GpsdPositionInformationProvider(GpsdPositionInformationProvider const&) = delete;
    GpsdPositionInformationProvider& operator=(GpsdPositionInformationProvider const&) = delete;
    GpsdPositionInformationProvider(GpsdPositionInformationProvider const&&) noexcept = delete;
    GpsdPositionInformationProvider& operator=(GpsdPositionInformationProvider&&) noexcept = delete;
    /**@cond Doxygen_Suppress */

    /**
     * @copy Rapid::Positioning::IGPSInformationProvider::getNumbersOfStatelite
     */
    std::uint8_t getNumbersOfStatelite() const override;

private:
    void onGpsPositionReceived();

    std::unique_ptr<Private::GpsdProvider> mProvider;
    System::FdNotifier mReadNotifier;
    KDBindings::ScopedConnection mReadNotifierConnection;
    int mReadFifo{-1};
    Common::GpsPositionData mLastPos;
    std::size_t mSatellites{0};
};

} // namespace Rapid::Positioning

#endif // !RAPID_POSITIONING_GPSDPOSITIONINFORMATIONPROVIDER_HPP
