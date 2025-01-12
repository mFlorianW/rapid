// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "VelocityData.hpp"

namespace Rapid::Common
{

struct SharedVelocityData : public SharedData
{
    double mVelocity = 0.0;

    friend bool operator==(SharedVelocityData const& lhs, SharedVelocityData const& rhs)
    {
        return lhs.mVelocity == rhs.mVelocity;
    }
};

VelocityData::VelocityData()
    : mData{new SharedVelocityData()}
{
}

VelocityData::VelocityData(double velocity)
    : VelocityData{}
{
    mData->mVelocity = velocity;
}

VelocityData::~VelocityData() = default;
VelocityData::VelocityData(VelocityData const&) = default;
VelocityData& VelocityData::operator=(VelocityData const&) = default;
VelocityData::VelocityData(VelocityData&&) noexcept = default;
VelocityData& VelocityData::operator=(VelocityData&&) noexcept = default;

double VelocityData::getVelocity() const noexcept
{
    return mData->mVelocity;
}

VelocityData VelocityData::createFromKmH(double kmh) noexcept
{
    return VelocityData{kmh / 3.6};
}

VelocityData VelocityData::createFromMpH(double mph) noexcept
{
    return VelocityData{mph * 0.44704};
}

bool operator==(VelocityData const& lhs, VelocityData const& rhs) noexcept
{
    return lhs.mData == rhs.mData || *lhs.mData == *rhs.mData;
}

bool operator!=(VelocityData const& lhs, VelocityData const& rhs) noexcept
{
    return !(lhs == rhs);
}

} // namespace Rapid::Common
