// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "SharedDataPointer.hpp"

namespace Rapid::Common
{
struct SharedVelocityData;

/**
 * Holds an velocity all returned values by this class are in the unit m/s.
 */
class VelocityData final
{
public:
    /**
     * Creates an VelocityData instance.
     * The velocity is set to 0.0;
     */
    VelocityData();

    /**
     * Creates an VelocityData instance.
     * Use the static methods @ref VelocityData::createFromKmH or
     * @ref VelocityData::createFormMpH when conversion is needed.
     * @param velocity The velocity must be in m/s
     */
    VelocityData(double velocity);

    /**
     * Default destructor
     */
    ~VelocityData();

    /**
     * Default copy operator
     */
    VelocityData(VelocityData const&);

    /**
     * Default copy asignment
     */
    VelocityData& operator=(VelocityData const&);

    /**
     * Default move operator
     */
    VelocityData(VelocityData&&) noexcept;

    /**
     * Default move asignment
     */
    VelocityData& operator=(VelocityData&&) noexcept;

    /**
     * Gives the stored velocity
     */
    double getVelocity() const noexcept;

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(VelocityData const& lhs, VelocityData const& rhs) noexcept;

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(VelocityData const& lhs, VelocityData const& rhs) noexcept;

    /**
     * Creates an VelocityData from value in km/h.
     * @param kmh The velocity value in kmh.
     * @return The created VelocityData.
     */
    static VelocityData createFromKmH(double kmh) noexcept;

    /**
     * Creates an VelocityData from value in mp/h.
     * @param kmh The velocity value in mp/h.
     * @return The created VelocityData.
     */
    static VelocityData createFromMpH(double mph) noexcept;

private:
    SharedDataPointer<SharedVelocityData> mData;
};
} // namespace Rapid::Common
