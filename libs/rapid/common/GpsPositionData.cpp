// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsPositionData.hpp"

namespace Rapid::Common
{

class SharedGpsPositionData : public SharedData
{
public:
    PositionData mPosition;
    Timestamp mTime;
    Date mDate;
    VelocityData mVelocity;

    friend bool operator==(SharedGpsPositionData const& lhs, SharedGpsPositionData const& rhs)
    {
        // clang-format off
        return ((lhs.mPosition) == (rhs.mPosition) &&
                (lhs.mTime) == (rhs.mTime) &&
                (lhs.mDate) == (rhs.mDate) &&
                (lhs.mVelocity) == (rhs.mVelocity));
        // clang-format on
    }
};

GpsPositionData::GpsPositionData()
    : mData(new SharedGpsPositionData{})
{
}

GpsPositionData::GpsPositionData(PositionData const& posData, Timestamp const& time, Date const& date)
    : mData{new SharedGpsPositionData{}}
{
    mData->mPosition = posData;
    mData->mTime = time;
    mData->mDate = date;
}

GpsPositionData::GpsPositionData(PositionData const& posData,
                                 Timestamp const& time,
                                 Date const& date,
                                 VelocityData velocity)
    : Rapid::Common::GpsPositionData{posData, time, date}
{
    mData->mVelocity = velocity;
}

GpsPositionData::~GpsPositionData() = default;

GpsPositionData::GpsPositionData(GpsPositionData const& other) = default;
GpsPositionData& GpsPositionData::operator=(GpsPositionData const& other) = default;
GpsPositionData::GpsPositionData(GpsPositionData&& other) = default;

GpsPositionData& GpsPositionData::operator=(GpsPositionData&& other)
{
    GpsPositionData moved{std::move(other)};
    std::swap(moved.mData, mData);
    return *this;
}

PositionData GpsPositionData::getPosition() const noexcept
{
    return mData->mPosition;
}

void GpsPositionData::setPosition(PositionData const& position)
{
    mData->mPosition = position;
}

Timestamp GpsPositionData::getTime() const noexcept
{
    return mData->mTime;
}

VelocityData GpsPositionData::getVelocity() const noexcept
{
    return mData->mVelocity;
}

void GpsPositionData::setTime(Timestamp const& time)
{
    mData->mTime = time;
}

Date GpsPositionData::getDate() const noexcept
{
    return mData->mDate;
}

void GpsPositionData::setDate(Date const& date)
{
    mData->mDate = date;
}

void GpsPositionData::setVelocity(VelocityData const& velocity)
{
    mData->mVelocity = velocity;
}

bool operator==(GpsPositionData const& lhs, GpsPositionData const& rhs)
{
    return lhs.mData == rhs.mData || *lhs.mData == *rhs.mData;
}

bool operator!=(GpsPositionData const& lhs, GpsPositionData const& rhs)
{
    return !(lhs == rhs);
}

} // namespace Rapid::Common
