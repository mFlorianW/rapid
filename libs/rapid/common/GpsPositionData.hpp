// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef POSITIONDATETIMEDATA_HPP
#define POSITIONDATETIMEDATA_HPP

#include "Date.hpp"
#include "PositionData.hpp"
#include "SharedDataPointer.hpp"
#include "Timestamp.hpp"
#include "VelocityData.hpp"

namespace Rapid::Common
{
class SharedGpsPositionData;

/**
 * A GpsPositionData always consists of a Position, Timestamp and Date.
 */
class GpsPositionData final
{
public:
    /**
     * Creates an empty instance of PositionDateTimeDate
     */
    GpsPositionData();

    /**
     * Creates an instance of the GpsPositionData.
     * @param posData The position data for the instance.
     * @param time The time data for the instance.
     * @param date The date data for the instance.
     */
    GpsPositionData(PositionData const& posData, Timestamp const& time, Date const& date);

    /**
     * Creates an instance of the GpsPositionData.
     * @param posData The position data for the instance.
     * @param time The time data for the instance.
     * @param date The date data for the instance.
     * @param velocity The velocity data for the instance.
     */
    GpsPositionData(PositionData const& posData, Timestamp const& time, Date const& date, VelocityData velocity);

    /**
     * Default destructor
     */
    ~GpsPositionData();

    /**
     * Copy constructor for GpsPositionData.
     * @param other The object to copy from.
     */
    GpsPositionData(GpsPositionData const& other);

    /**
     * Copy assignment operator for GpsPositionData.
     * @param other The object to copy from.
     * @return PositionData& A reference to the copied instance.
     */
    GpsPositionData& operator=(GpsPositionData const& other);

    /**
     * The move constructor for GpsPositionData.
     * @param other The object to move from.
     */
    GpsPositionData(GpsPositionData&& other);

    /**
     * The move assignment operator for GpsPositionData.
     * @param other The object to move from.
     * @return PositionData& A reference of the moved instance.
     */
    GpsPositionData& operator=(GpsPositionData&& other);

    /**
     * @return The current position
     */
    PositionData getPosition() const noexcept;

    /**
     * Sets a new position.
     * @param position The new position.
     */
    void setPosition(PositionData const& position);

    /**
     * @return The current time
     */
    Timestamp getTime() const noexcept;

    /**
     * @return Gives the velocity.
     */
    VelocityData getVelocity() const noexcept;

    /**
     * Sets a new time.
     * @param time The new time.
     */
    void setTime(Timestamp const& time);

    /**
     * @return The current date.
     */
    Date getDate() const noexcept;

    /**
     * Sets a new date.
     * @param date The new date.
     */
    void setDate(Date const& date);

    /**
     * Sets the velocity
     * @param velocity The new velocity
     */
    void setVelocity(VelocityData const& velocity);

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(GpsPositionData const& lhs, GpsPositionData const& rhs);

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(GpsPositionData const& lhs, GpsPositionData const& rhs);

private:
    SharedDataPointer<SharedGpsPositionData> mData;
};

} // namespace Rapid::Common

#endif // POSITIONDATETIMEDATA_HPP
