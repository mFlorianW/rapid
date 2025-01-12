// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SimpleLaptimer.hpp"
#include "DistanceCalculator.hpp"
#include <algorithm>

using namespace Rapid::Common;

namespace Rapid::Algorithm
{

SimpleLaptimer::SimpleLaptimer() = default;

void SimpleLaptimer::setTrack(Common::TrackData const& track)
{
    mTrackData = track;
}

void SimpleLaptimer::updatePositionAndTime(Common::GpsPositionData const& data)
{
    mCurrentPoints.push_front(data.getPosition());
    if (mCurrentPoints.size() > 4) {
        mCurrentPoints.pop_back();
    } else if (mCurrentPoints.size() < 4) {
        return;
    }

    // Update currentLaptime
    if (mLapState != LapState::WaitingForFirstStart) {
        auto const lapTime = data.getTime() - mLapStartedTimestamp;
        currentLaptime.set(lapTime);

        auto const sectorTime = data.getTime() - mSectorStartedTimestamp;
        currentSectorTime.set(sectorTime);
    }

    if (mLapState == LapState::WaitingForFirstStart) {
        auto const useFinishLineAsStartline = mTrackData.getStartline() == Common::PositionData{};
        auto const startLine = useFinishLineAsStartline ? mTrackData.getFinishline() : mTrackData.getStartline();
        if (passedPoint(startLine)) {
            mLapState =
                (mTrackData.getNumberOfSections() > 0) ? LapState::IteratingTrackPoints : LapState::WaitingForFinish;
            mCurrentTrackPoint = 0;
            currentLaptime.set(Timestamp{"00:00:00.000"});
            currentSectorTime.set(Timestamp{"00:00:00.000"});
            mLapStartedTimestamp = data.getTime();
            mSectorStartedTimestamp = data.getTime();
            lapStarted.emit();
        }
    } else if (mLapState == LapState::IteratingTrackPoints) {
        if (passedPoint(mTrackData.getSection(mCurrentTrackPoint))) {
            ++mCurrentTrackPoint;
            if (mCurrentTrackPoint >= mTrackData.getNumberOfSections()) {
                mLapState = LapState::WaitingForFinish;
            }
            mLastSectorTime = currentSectorTime.get();
            mSectorStartedTimestamp = data.getTime();
            currentSectorTime.set(Timestamp{"00:00:00.000"});
            sectorFinished.emit();
        }
    } else if (mLapState == LapState::WaitingForFinish) {
        auto const finishLine = mTrackData.getFinishline();
        if (passedPoint(finishLine)) {
            mLastLapTime = currentLaptime.get();
            mLastSectorTime = currentSectorTime.get();
            mLapStartedTimestamp = data.getTime();
            mSectorStartedTimestamp = data.getTime();
            currentLaptime.set(Timestamp{"00:00:00.000"});
            currentSectorTime.set(Timestamp{"00:00:00.000"});

            // TODO: Only works with with a circuit. Additional check is needed in the future when the finish line
            // and start line is not the same.
            if (mTrackData.getNumberOfSections() > 0) {
                mCurrentTrackPoint = 0;
                mLapState = LapState::IteratingTrackPoints;
            } else {
                mCurrentPoints.clear();
            }

            lapFinished.emit();
            lapStarted.emit();
        }
    }
}

Common::Timestamp SimpleLaptimer::getLastLaptime() const
{
    return mLastLapTime;
}

Common::Timestamp SimpleLaptimer::getLastSectorTime() const
{
    return mLastSectorTime;
}

bool SimpleLaptimer::passedPoint(Common::PositionData const& point) const
{
    constexpr std::uint8_t range = 50;
    bool pointsInRange = std::all_of(mCurrentPoints.cbegin(), mCurrentPoints.cend(), [=](Common::PositionData pos) {
        return DistanceCalculator::calculateDistance(pos, point) <= range;
    });

    if (!pointsInRange) {
        return false;
    }

    std::array<float, 4> distances{};
    for (size_t i = 0; i < 4; ++i) {
        distances[i] = DistanceCalculator::calculateDistance(mCurrentPoints[i], point);
    }

    bool lastDistance = distances[2] < distances[3];
    bool firstDistance = distances[0] > distances[1];
    if (firstDistance and lastDistance and ((distances[1] < distances[2]) or (distances[1] > distances[2]))) {
        return true;
    }

    return false;
}

} // namespace Rapid::Algorithm
