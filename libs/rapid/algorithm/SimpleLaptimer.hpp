// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SIMPLELAPTIMER_HPP
#define SIMPLELAPTIMER_HPP

#include "ILaptimer.hpp"
#include <deque>

namespace Rapid::Algorithm
{

class SimpleLaptimer final : public ILaptimer
{
public:
    /**
     * Default constructor
     */
    SimpleLaptimer();

    /**
     * Disabled copy operator
     */
    SimpleLaptimer(SimpleLaptimer const&) = delete;

    /**
     * Disabled copy operator
     */
    SimpleLaptimer& operator=(SimpleLaptimer const&) = delete;

    /**
     * Default move operator
     */
    SimpleLaptimer(SimpleLaptimer&&) noexcept = default;

    /**
     * Default move operator
     */
    SimpleLaptimer& operator=(SimpleLaptimer&&) noexcept = default;

    /**
     * Default destructor
     */
    ~SimpleLaptimer() override = default;

    /**
     * @copydoc SimpleLaptimer::setTrack(const Common::TrackData &track)
     */
    void setTrack(Common::TrackData const& track) override;

    /**
     * @copydoc updatePositionAndTime(const Common::GpsPositionData &data)
     */
    void updatePositionAndTime(Common::GpsPositionData const& data) override;

    /**
     * @copydoc SimpleLaptimer::getLastLaptime()
     */
    Common::Timestamp getLastLaptime() const override;

    /**
     * @copydoc SimpleLaptimer::getLastLaptime()
     */
    Common::Timestamp getLastSectorTime() const override;

private:
    /**
     * Checks if the last 4 positions stored in mCurrentPoints passed the point specified.
     * @param point The point the shall be checked against the last 4 known positions.
     * @return True if the point specified was passed, False otherwise.
     */
    bool passedPoint(Common::PositionData const& point) const;

private:
    Common::TrackData mTrackData;
    size_t mCurrentTrackPoint{0};

    /* This double ended que contains the last 4 track points. The point at position 0 is the latest one
    and the point at position 3 is the oldest respectively. */
    std::deque<Common::PositionData> mCurrentPoints;

    enum LapState
    {
        WaitingForFirstStart,
        IteratingTrackPoints,
        WaitingForFinish
    };

    LapState mLapState{WaitingForFirstStart};
    Common::Timestamp mLapStartedTimestamp;
    Common::Timestamp mLastLapTime;
    Common::Timestamp mSectorStartedTimestamp;
    Common::Timestamp mLastSectorTime;
};

} // namespace Rapid::Algorithm

#endif // SIMPLELAPTIMER_HPP
