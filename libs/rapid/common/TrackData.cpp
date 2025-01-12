// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "TrackData.hpp"

namespace Rapid::Common
{

class SharedTrackData : public SharedData
{
public:
    std::string mTrackName;
    PositionData mFinishline;
    PositionData mStartline;
    std::vector<PositionData> mSections;

    friend bool operator==(SharedTrackData const& lhs, SharedTrackData const& rhs)
    {
        // clang-format off
        return ((lhs.mTrackName) == (rhs.mTrackName) &&
                (lhs.mFinishline) == (rhs.mFinishline) &&
                (lhs.mStartline) == (rhs.mStartline) &&
                (lhs.mSections) == (rhs.mSections));
        // clang-format on
    }
};

TrackData::TrackData()
    : mData{new SharedTrackData{}}
{
}

TrackData::~TrackData() = default;
TrackData::TrackData(TrackData const& ohter) = default;
TrackData& TrackData::operator=(TrackData const& other) = default;
TrackData::TrackData(TrackData&& other) = default;

TrackData& TrackData::operator=(TrackData&& other)
{
    TrackData moved{std::move(other)};
    std::swap(mData, moved.mData);
    return *this;
}

std::string const& TrackData::getTrackName() const
{
    return mData->mTrackName;
}

void TrackData::setTrackName(std::string const& name)
{
    mData->mTrackName = name;
}

PositionData const& TrackData::getStartline() const
{
    return mData->mStartline;
}

void TrackData::setStartline(PositionData const& startline)
{
    mData->mStartline = startline;
}

PositionData const& TrackData::getFinishline() const
{
    return mData->mFinishline;
}

void TrackData::setFinishline(PositionData const& finishline)
{
    mData->mFinishline = finishline;
}

size_t TrackData::getNumberOfSections() const
{
    return mData->mSections.size();
}

PositionData const& TrackData::getSection(size_t sectionIndex) const
{
    return mData->mSections.at(sectionIndex);
}

std::vector<PositionData> const& TrackData::getSections() const
{
    return mData->mSections;
}

void TrackData::setSections(std::vector<PositionData> const& sections)
{
    mData->mSections = sections;
}

bool operator==(TrackData const& lhs, TrackData const& rhs)
{
    return lhs.mData == rhs.mData || *lhs.mData == *rhs.mData;
}

bool operator!=(TrackData const& lhs, TrackData const& rhs)
{
    return !(lhs == rhs);
}

} // namespace Rapid::Common
