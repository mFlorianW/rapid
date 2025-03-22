// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Date.hpp"
#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <sstream>

namespace Rapid::Common
{

class SharedDate : public SharedData
{
public:
    std::uint16_t mYear{0};
    std::uint8_t mMonth{0};
    std::uint8_t mDay{0};

    friend bool operator==(SharedDate const& lhs, SharedDate const& rhs)
    {
        // clang-format off
        return ((lhs.mYear == rhs.mYear) &&
                (lhs.mMonth == rhs.mMonth) &&
                (lhs.mDay == rhs.mDay));
        // clang-format on
    }

    friend bool operator!=(SharedDate const& lhs, SharedDate const& rhs)
    {
        return !(lhs == rhs);
    }

    friend bool operator<(SharedDate const& lhs, SharedDate const& rhs)
    {
        if (lhs.mYear != rhs.mYear) {
            return lhs.mYear < rhs.mYear;
        } else if (lhs.mMonth != rhs.mMonth) {
            return lhs.mMonth < rhs.mMonth;
        }
        return lhs.mDay < rhs.mDay;
    }

    friend bool operator>(SharedDate const& lhs, SharedDate const& rhs)
    {
        return rhs < lhs;
    }
};

Date::Date()
    : mData{new SharedDate}
{
}

Date::Date(std::string const& dateString)
    : mData{new SharedDate}
{
    std::istringstream input(dateString);
    std::array<std::string, 3> splittedStrings;
    for (auto& splittedString : splittedStrings) {
        getline(input, splittedString, '.');
    }

    try {
        mData->mDay = static_cast<std::uint8_t>(std::stoi(splittedStrings[0]));
        mData->mMonth = static_cast<std::uint8_t>(std::stoi(splittedStrings[1]));
        mData->mYear = static_cast<std::uint16_t>(std::stoi(splittedStrings[2]));
    } catch (std::invalid_argument const& e) {
        spdlog::error("Invalid argument passed. {}", dateString);
    } catch (std::out_of_range const& e) {
        spdlog::error("Out of range during converting.{}", dateString);
    }
}

Date::~Date() = default;
Date::Date(Date const& other) = default;
Date& Date::operator=(Date const& other) = default;
Date::Date(Date&& other) = default;
Date& Date::operator=(Date&& other) = default;

uint16_t Date::getYear() const noexcept
{
    return mData->mYear;
}

void Date::setYear(uint16_t year)
{
    mData->mYear = year;
}

uint8_t Date::getMonth() const noexcept
{
    return mData->mMonth;
}

void Date::setMonth(uint8_t month)
{
    mData->mMonth = month;
}

uint8_t Date::getDay() const noexcept
{
    return mData->mDay;
}

void Date::setDay(uint8_t day)
{
    mData->mDay = day;
}

std::string Date::asString() const noexcept
{
    std::ostringstream dateAsString;

    // clang-format off
    dateAsString << std::setw(2) << std::setfill('0') << std::to_string(getDay()) << "."
                 << std::setw(2) << std::setfill('0') << std::to_string(getMonth()) << "."
                 << std::setw(4) << std::setfill('0') << std::to_string(getYear());
    // clang-format on

    return dateAsString.str();
}

Date Date::getSystemDate() noexcept
{
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    auto const time = std::localtime(&timeT);

    auto date = Date{};
    date.setYear(1900 + time->tm_year); // The mYear is relativ to 1900.
    date.setMonth(++time->tm_mon); // The month is in the range 0-11.
    date.setDay(time->tm_mday);
    return date;
}

bool operator<(Date const& lhs, Date const& rhs)
{
    return *lhs.mData < *rhs.mData;
}

bool operator>(Date const& lhs, Date const& rhs)
{
    return *lhs.mData > *rhs.mData;
}

bool operator==(Date const& lhs, Date const& rhs)
{
    return lhs.mData == rhs.mData || *(lhs.mData) == *(rhs.mData);
}

bool operator!=(Date const& lhs, Date const& rhs)
{
    return !(lhs == rhs);
}

} // namespace Rapid::Common
