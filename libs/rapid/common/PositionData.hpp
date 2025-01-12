// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "SharedDataPointer.hpp"

namespace Rapid::Common
{

class SharedPositionData;
class PositionData final
{
public:
    /**
     * Creates an instance of PositionData.
     */
    PositionData();

    /**
     * Creates a PositionData instance with given latitude and longitude.
     * @param latitude The latitude of the PositionData.
     * @param longitude The longitude of the PositionData.
     */
    PositionData(float latitude, float longitude);

    /**
     * Default destructor.
     */
    ~PositionData();

    /**
     * Copy constructor for PositionData.
     * @param other The object to copy from.
     */
    PositionData(PositionData const& other);

    /**
     * Copy assignment operator for PositionData.
     * @param other The object to copy from.
     * @return PositionData& A reference to the copied instance.
     */
    PositionData& operator=(PositionData const& other);

    /**
     * The move constructor for PositionData.
     * @param other The object to move from.
     */
    PositionData(PositionData&& other);

    /**
     * The move assignment operator for PositionData.
     * @param other The object to move from.
     * @return PositionData& A reference of the moved instance.
     */
    PositionData& operator=(PositionData&& other);

    /**
     * Gives the latitude.
     * @return float The latitude.
     */
    float getLatitude() const;

    /**
     * Sets a new latitude value.
     * @param latitude The Latitude value.
     */
    void setLatitude(float latitude);

    /**
     * Gives the longitude.
     * @return float The longitude.
     */
    float getLongitude() const;

    /**
     * Sets a new longitude value.
     * @param longitude The new longitude value.
     */
    void setLongitude(float longitude);

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(PositionData const& lhs, PositionData const& rhs);

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(PositionData const& lhs, PositionData const& rhs);

private:
    SharedDataPointer<SharedPositionData> mData;
};

} // namespace Rapid::Common
