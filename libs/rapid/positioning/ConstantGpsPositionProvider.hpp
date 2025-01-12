// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "positioning/IGpsPositionProvider.hpp"
#include <system/Timer.hpp>

namespace Rapid::Positioning
{

/**
 * This @ref ConstantGpsPositionProvider replays a list of GPS positions at a 10Hz frequency. The replayed
 * positions are based on the provided list. Positions are automatically interpolated when the two
 * positions are to far of each other. The interpolated position is based on set veclocity.
 */
class ConstantGpsPositionProvider final : public IGpsPositionProvider
{
public:
    /**
     * Creates an instance of the FileBasedPositionDateTimeProvider.
     * @param gpsPositions The GPS position list that is used for the replay.
     */
    explicit ConstantGpsPositionProvider(std::vector<Common::PositionData> const& gpsPositions = {});

    /**
     * Empty default destructor
     */
    ~ConstantGpsPositionProvider() override;

    /**
     * Disabled copy operator
     */
    ConstantGpsPositionProvider(ConstantGpsPositionProvider const&) = delete;

    /**
     * Disabled copy operator
     */
    ConstantGpsPositionProvider& operator=(ConstantGpsPositionProvider const&) = delete;

    /**
     * Disabled move operator
     */
    ConstantGpsPositionProvider(ConstantGpsPositionProvider&&) noexcept = delete;

    /**
     * Disabled move operator
     */
    ConstantGpsPositionProvider& operator=(ConstantGpsPositionProvider&&) noexcept = delete;

    /**
     * Overwrites the GPS positions for playback.
     * @param gpsPositions The GPS position list that is used for the replay.
     */
    void setGpsPositions(std::vector<Common::PositionData> const& gpsPositions);

    /**
     * Sets the speed for the GPS position updates.
     * It's only possible to update the speed when the provider is not running.
     * @param speed The speed in meter per second.
     */
    void setVelocityInMeterPerSecond(float velocity);

    /**
     * Starts the GPS position date time provider.
     */
    void start();

    /**
     * Stops the GPS position date time provider.
     */
    void stop();

private:
    void convertTrackPoints(std::vector<Common::PositionData> const& gpsPositions);
    void handleGPSPositionTick();

private:
    struct Point
    {
        double x = 0.0f;
        double y = 0.0f;
        std::array<char, 3> zone = {0};
    };

    float mSpeed{0};
    System::Timer mTickTimer;
    std::vector<Point> mTrackData;
    std::vector<Point>::const_iterator mTrackDataIt;
    Point mCurrentPosition;
};

} // namespace Rapid::Positioning
