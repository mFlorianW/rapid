// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "SharedDataPointer.hpp"
#include <cstdint>
#include <string>

namespace Rapid::Common
{
class SharedTimestamp;
class Timestamp
{
public:
    /**
     * Creates an instance of Timestamp
     */
    Timestamp();

    /**
     * Create an instance of Timestamp by a string.
     * The string must have the format of hh:mm:ss.nnn
     * @param timestampString
     */
    Timestamp(std::string const& timestampString);

    /**
     * Default destructor
     */
    ~Timestamp();

    /**
     * Copy constructor for Timestamp.
     * @param other  The object to copy from.
     */
    Timestamp(Timestamp const& other);

    /**
     * The copy assignment operator for Timestamp.
     * @param other The object to copy from.
     * @return Timestamp& A reference to the copied Timestamp.
     */
    Timestamp& operator=(Timestamp const& other);

    /**
     * The move constructor for Timestamp.
     * @param other The object to move from.
     */
    Timestamp(Timestamp&& other);

    /**
     * The move assignment operator for timestamp.
     * @param other The object to move from.
     * @return Timestamp& A reference to the moved Timestamp.
     */
    Timestamp& operator=(Timestamp&& other);

    /**
     * Gives the hour.
     * @return std::uint8_t The hour of Timestamp.
     */
    std::uint8_t getHour() const;

    /**
     * Sets a new hour.
     * @param hour The new hour.
     */
    void setHour(std::uint8_t hour);

    /**
     * Gives the minute.
     * @return std::uint8_t The minute of Timestamp.
     */
    std::uint8_t getMinute() const;

    /**
     * Sets a new minute.
     * @param minute The new minute.
     */
    void setMinute(std::uint8_t minute);

    /**
     * Gives the second of the Timestamp.
     * @return std::uint8_t The second of Timestamp.
     */
    std::uint8_t getSecond() const;

    /**
     * Sets a new second.
     * @param second The new second.
     */
    void setSecond(std::uint8_t second);

    /**
     * Gives the fractional of a second.
     * @return std::uint16_t The fractional of a second.
     */
    std::uint16_t getFractionalOfSecond() const;

    /**
     * Sets a new fractional of second in Timestamp.
     * @param fractionalOfSecond The new fractional of second.
     */
    void setFractionalOfSecond(std::uint16_t fractionalOfSecond);

    /**
     * Converts the time compontes of the timestamp to a string.
     * The string will have the format of: 00:01:35.123
     * @return The time component to the timestamp as string.
     */
    std::string asString() const noexcept;

    /**
     * Makes the addition between two timestamps and returs the result.
     * @note The time will wrap when passing midnight.
     * @param rhs The right hand side operator of the addition.
     * @return A new Timestamp with the result of the plus operation.
     */
    Timestamp operator+(Timestamp const& rhs) const noexcept;

    /**
     * Make the subtraction of the given timestamp.
     * @note The time will wrap when passing midnight.
     * @param rhs The right hand side operator of the subtraction.
     * @return A new Timestamp with the result of the minus operation.
     */
    Timestamp operator-(Timestamp const& rhs) const noexcept;

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(Timestamp const& lhs, Timestamp const& rhs);

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(Timestamp const& lhs, Timestamp const& rhs);

    /**
     * Gives the system time stamp
     * @return A timestamp object with the system time.
     */
    static Timestamp getSystemTimestamp();

private:
    std::int32_t convertToMilliSeconds() const;

private:
    SharedDataPointer<SharedTimestamp> mData;
};

} // namespace Rapid::Common
