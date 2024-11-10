// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ConstantVelocityPositionDateTimeProvider.hpp"
#include <UTM.hpp>
#include <sys/time.h>

using namespace Rapid::Common;

namespace Rapid::Positioning
{

ConstantVelocityPositionDateTimeProvider::ConstantVelocityPositionDateTimeProvider(
    std::vector<Common::PositionData> const& gpsPositions)
{
    // setup timer
    mTickTimer.setInterval(std::chrono::milliseconds(100));
    mTickTimer.timeout.connect(&ConstantVelocityPositionDateTimeProvider::handleGPSPositionTick, this);

    convertTrackPoints(gpsPositions);
}

void ConstantVelocityPositionDateTimeProvider::setGpsPositions(std::vector<Common::PositionData> const& gpsPositions)
{
    convertTrackPoints(gpsPositions);
}

ConstantVelocityPositionDateTimeProvider::~ConstantVelocityPositionDateTimeProvider() = default;

void ConstantVelocityPositionDateTimeProvider::setVelocityInMeterPerSecond(float speed)
{
    mSpeed = speed;
}

void ConstantVelocityPositionDateTimeProvider::start()
{
    mTickTimer.start();
}

void ConstantVelocityPositionDateTimeProvider::stop()
{
    mTickTimer.stop();
}

void ConstantVelocityPositionDateTimeProvider::convertTrackPoints(std::vector<Common::PositionData> const& gpsPositions)
{
    if (gpsPositions.empty()) {
        return;
    }

    // convert positions to UTM for easier calculations
    Point point;
    for (auto const& posData : gpsPositions) {
        UTM::LLtoUTM(posData.getLatitude(), posData.getLongitude(), point.x, point.y, point.zone.data());
        mTrackData.push_back(point);
    }

    mTrackDataIt = mTrackData.cbegin();
    mCurrentPosition = *mTrackData.cbegin();
}

void ConstantVelocityPositionDateTimeProvider::handleGPSPositionTick()
{
    if (mTrackData.empty()) {
        std::cout << "ConstantVelocityPositionDateTimeProvider do nothing: PositionData Empty" << std::endl;
        return;
    }

    if ((mTrackDataIt != mTrackData.cend()) && (mTrackDataIt != mTrackData.begin())) {
        auto p0 = *mTrackDataIt;
        // Calculate the direction vector between our current position and the target point
        Point direction{p0.x - mCurrentPosition.x, p0.y - mCurrentPosition.y};

        // Calculate the length of our direction vector
        auto length = static_cast<float>(std::sqrt(direction.x * direction.x + direction.y * direction.y));

        // Normalize direction
        Point normalizedDirection{direction.x / length, direction.y / length};

        // Calculate distance we moved based on speed (meters per second) and elapsed time (seconds)
        auto time = static_cast<float>(mTickTimer.getInterval().count()) / 1000.0f;
        Point distanceTraveled{normalizedDirection.x * time * mSpeed, normalizedDirection.y * time * mSpeed};

        // Calculate our new position
        mCurrentPosition.x += distanceTraveled.x;
        mCurrentPosition.y += distanceTraveled.y;

        // If we overran our target point just move on to the next one :)
        direction.x = p0.x - mCurrentPosition.x;
        direction.y = p0.y - mCurrentPosition.y;
        auto newLength = static_cast<float>(std::sqrt(direction.x * direction.x + direction.y * direction.y));
        if (newLength > length) {
            mTrackDataIt = mTrackDataIt + 2;
            if (mTrackDataIt == mTrackData.cend()) {
                mTrackDataIt = mTrackData.cbegin();
            }
        }
    } else if (mTrackDataIt == mTrackData.cbegin()) {
        mCurrentPosition = *mTrackDataIt;
        ++mTrackDataIt;
    } else {
        mTrackDataIt = mTrackData.cbegin();
    }

    auto position = GpsPositionData{};

    // Sett the position
    double lat = 0.0f;
    double longi = 0.0f;
    UTM::UTMtoLL(mCurrentPosition.x, mCurrentPosition.y, mCurrentPosition.zone.data(), lat, longi);
    position.setPosition(PositionData{static_cast<float>(lat), static_cast<float>(longi)});

    // Set the time
    position.setTime(Timestamp::getSystemTimestamp());

    // Set the date.
    position.setDate(Date::getSystemDate());

    // Set velocity.
    position.setVelocity(VelocityData{mSpeed});

    // Set the new PositionDateTime
    gpsPosition.set(position);
}

} // namespace Rapid::Positioning
