// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LAPTIMERCORE_STATICGPSINFORMATIONPROVIDER_HPP
#define LAPTIMERCORE_STATICGPSINFORMATIONPROVIDER_HPP

#include "IGPSInformationProvider.hpp"
namespace Rapid::Positioning
{

class StaticGpsInformationProvider : public IGpsInformationProvider
{
public:
    GpsFixMode getGpsFixMode() const noexcept override
    {
        return GpsFixMode::Fix3d;
    }

    uint8_t getNumbersOfStatelite() const override
    {
        return 0;
    }
};

} // namespace Rapid::Positioning
#endif // LAPTIMERCORE_STATICGPSINFORMATIONPROVIDER_HPP
