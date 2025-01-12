// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionMetaData.hpp"

namespace Rapid::Common
{

class SharedSessionMetaData : public SharedData
{
public:
    Date mSessionDate;
    Timestamp mSessionTime;
    TrackData mSessionTrack;
    std::size_t mId = 0;

    friend bool operator==(SharedSessionMetaData const& lhs, SharedSessionMetaData const& rhs)
    {
        // clang-format off
        return (
                (lhs.mSessionDate) == (rhs.mSessionDate) and
                (lhs.mSessionTime) == (rhs.mSessionTime) and
                (lhs.mSessionTrack) == (rhs.mSessionTrack) and
                (lhs.mId) == (rhs.mId)
               );
        // clang-format on
    }
};

SessionMetaData::SessionMetaData()
    : mData{new SharedSessionMetaData{}}
{
}

SessionMetaData::SessionMetaData(TrackData track, Date date, Timestamp time, std::size_t id)
    : mData{new SharedSessionMetaData}
{
    mData->mSessionTrack = track;
    mData->mSessionTime = time;
    mData->mSessionDate = date;
    mData->mId = id;
}

SessionMetaData::~SessionMetaData() = default;
SessionMetaData::SessionMetaData(SessionMetaData const&) = default;
SessionMetaData& SessionMetaData::operator=(SessionMetaData const&) = default;
SessionMetaData::SessionMetaData(SessionMetaData&&) noexcept = default;
SessionMetaData& SessionMetaData::operator=(SessionMetaData&&) noexcept = default;

std::size_t SessionMetaData::getId() const noexcept
{
    return mData->mId;
}

Date SessionMetaData::getSessionDate() const noexcept
{
    return mData->mSessionDate;
}

Timestamp SessionMetaData::getSessionTime() const noexcept
{
    return mData->mSessionTime;
}

TrackData const& SessionMetaData::getTrack() const noexcept
{
    return mData->mSessionTrack;
}

bool SessionMetaData::operator==(SessionMetaData const& rhs) const noexcept
{
    return mData == rhs.mData || *mData == *rhs.mData;
}

bool SessionMetaData::operator!=(SessionMetaData const& rhs) const noexcept
{
    return !(*this == rhs);
}

} // namespace Rapid::Common
