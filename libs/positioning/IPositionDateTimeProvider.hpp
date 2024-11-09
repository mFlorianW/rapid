// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef IPOSITIONDATETIMEPROVIDER_HPP
#define IPOSITIONDATETIMEPROVIDER_HPP

#include "GpsPositionData.hpp"
#include <kdbindings/property.h>

namespace Rapid::Positioning
{

class IPositionDateTimeProvider
{
public:
    /**
     *  Virtual default destructor.
     */
    virtual ~IPositionDateTimeProvider() = default;

    /**
     * Disabled copy constructor
     */
    IPositionDateTimeProvider(IPositionDateTimeProvider const&) = delete;

    /**
     * Disabled copy operator
     */
    IPositionDateTimeProvider& operator=(IPositionDateTimeProvider const&) = delete;

    /**
     * Default move operator
     */
    IPositionDateTimeProvider(IPositionDateTimeProvider&&) noexcept = default;

    /**
     * Default move operator
     */
    IPositionDateTimeProvider& operator=(IPositionDateTimeProvider&&) noexcept = default;

    /**
     * This property holds current PostionDateTimeData
     */
    KDBindings::Property<Common::GpsPositionData> positionTimeData;

protected:
    /**
     * Default protected constructor.
     */
    IPositionDateTimeProvider() = default;
};

} // namespace Rapid::Positioning

#endif // IPOSITIONDATETIMEPROVIDER_HPP
