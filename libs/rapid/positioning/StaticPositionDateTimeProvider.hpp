// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef STATICPOSITIONDATETIMEPROVIDER_HPP
#define STATICPOSITIONDATETIMEPROVIDER_HPP

#include "IGpsPositionProvider.hpp"
#include <common/Timestamp.hpp>

namespace Rapid::Positioning
{

class StaticPositionDateTimeProvider final : public IGpsPositionProvider
{
public:
    StaticPositionDateTimeProvider()
    {
        gpsPosition.set(Common::GpsPositionData{{52.0270889, 11.2803483}, Common::Timestamp{"15:05:10.234"}, {}});
    }

    /**
     * Default Destructor
     */
    ~StaticPositionDateTimeProvider() override = default;

    /**
     * Disabled copy operator
     */
    StaticPositionDateTimeProvider(StaticPositionDateTimeProvider const&) = delete;

    /**
     * Disabled copy operator
     */
    StaticPositionDateTimeProvider& operator=(StaticPositionDateTimeProvider const&) = delete;

    /**
     * Default move operator
     */
    StaticPositionDateTimeProvider(StaticPositionDateTimeProvider&&) noexcept = default;

    /**
     * Default move operator
     */
    StaticPositionDateTimeProvider& operator=(StaticPositionDateTimeProvider&&) noexcept = default;
};

} // namespace Rapid::Positioning

#endif // STATICPOSITIONDATETIMEPROVIDER_HPP
