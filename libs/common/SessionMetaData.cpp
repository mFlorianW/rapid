// SPDX-FileCopyrightText: 2024 All contributors
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

    friend bool operator==(SharedSessionMetaData const& lhs, SharedSessionMetaData const& rhs)
    {
        // clang-format off
        return ((lhs.mSessionDate) == (rhs.mSessionDate) &&
                (lhs.mSessionTime) == (rhs.mSessionTime) &&
                (lhs.mSessionTrack) == (rhs.mSessionTrack));
        // clang-format on
    }
};

SessionMetaData::SessionMetaData()
    : mData{new SharedSessionMetaData{}}
{
}

SessionMetaData::SessionMetaData(TrackData track, Date date, Timestamp time)
    : mData{new SharedSessionMetaData}
{
    mData->mSessionTrack = track;
    mData->mSessionTime = time;
    mData->mSessionDate = date;
}

SessionMetaData::~SessionMetaData() = default;
SessionMetaData::SessionMetaData(SessionMetaData const&) = default;
SessionMetaData& SessionMetaData::operator=(SessionMetaData const&) = default;
SessionMetaData::SessionMetaData(SessionMetaData&&) noexcept = default;
SessionMetaData& SessionMetaData::operator=(SessionMetaData&&) noexcept = default;

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

} // namespace Rapid::Common
