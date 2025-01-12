// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionData.hpp"

namespace Rapid::Common
{

class SharedSessionData : public SharedData
{
public:
    std::vector<LapData> mLaps;

    friend bool operator==(SharedSessionData const& lhs, SharedSessionData const& rhs)
    {
        return lhs.mLaps == rhs.mLaps;
    }
};

SessionData::SessionData()
    : SessionMetaData{}
    , mData{new SharedSessionData}
{
}

SessionData::SessionData(TrackData const& track, Date const& sessionDate, Timestamp const& sessionTime, std::size_t id)
    : SessionMetaData{track, sessionDate, sessionTime, id}
    , mData{new SharedSessionData}
{
}

SessionData::~SessionData() = default;
SessionData::SessionData(SessionData const& other) = default;
SessionData& SessionData::operator=(SessionData const& other) = default;
SessionData::SessionData(SessionData&& other) = default;
SessionData& SessionData::operator=(SessionData&& other) = default;

std::size_t SessionData::getNumberOfLaps() const noexcept
{
    return mData->mLaps.size();
}

std::optional<LapData> SessionData::getLap(std::size_t index) const noexcept
{
    if (index > mData->mLaps.size()) {
        return std::nullopt;
    }

    return {mData->mLaps.at(index)};
}

std::vector<LapData> const& SessionData::getLaps() const noexcept
{
    return mData->mLaps;
}

void SessionData::addLap(LapData const& lap)
{
    mData->mLaps.push_back(lap);
}

void SessionData::addLaps(std::vector<LapData> const& laps)
{
    for (auto const& lap : laps) {
        mData->mLaps.push_back(lap);
    }
}

bool operator==(SessionData const& lhs, SessionData const& rhs)
{
    return lhs.mData == rhs.mData || *lhs.mData == *rhs.mData;
}

bool operator!=(SessionData const& lhs, SessionData const& rhs)
{
    return !(lhs == rhs);
}

} // namespace Rapid::Common
