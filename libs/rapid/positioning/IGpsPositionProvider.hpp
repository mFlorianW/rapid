// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef IPOSITIONDATETIMEPROVIDER_HPP
#define IPOSITIONDATETIMEPROVIDER_HPP

#include "common/GpsPositionData.hpp"
#include <kdbindings/property.h>

namespace Rapid::Positioning
{

class IGpsPositionProvider
{
public:
    /**
     *  Virtual default destructor.
     */
    virtual ~IGpsPositionProvider() = default;

    /**
     * Disabled copy constructor
     */
    IGpsPositionProvider(IGpsPositionProvider const&) = delete;

    /**
     * Disabled copy operator
     */
    IGpsPositionProvider& operator=(IGpsPositionProvider const&) = delete;

    /**
     * Default move operator
     */
    IGpsPositionProvider(IGpsPositionProvider&&) noexcept = default;

    /**
     * Default move operator
     */
    IGpsPositionProvider& operator=(IGpsPositionProvider&&) noexcept = default;

    /**
     * This property holds current PostionDateTimeData
     */
    KDBindings::Property<Common::GpsPositionData> gpsPosition;

protected:
    /**
     * Default protected constructor.
     */
    IGpsPositionProvider() = default;
};

} // namespace Rapid::Positioning

#endif // IPOSITIONDATETIMEPROVIDER_HPP
