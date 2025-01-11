// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "common/Date.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Common;

TEST_CASE("The Date shall be convertable to a string in format DD.MM.YYYY")
{
    constexpr auto expectedResult = "01.01.1970";
    Date date;
    date.setDay(1);
    date.setMonth(1);
    date.setYear(1970);

    auto result = date.asString();

    REQUIRE(result == expectedResult);
}

TEST_CASE("The Date shall be creatable from string in the format of dd.MM.YYYY")
{
    Date expectedDate;
    expectedDate.setDay(1);
    expectedDate.setMonth(1);
    expectedDate.setYear(1970);

    auto date = Date{"01.01.1970"};
    REQUIRE(date == expectedDate);
}