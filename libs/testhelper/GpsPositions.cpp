// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GpsPositions.hpp"

using namespace Rapid::Common;

namespace Rapid::TestHelper::GpsPositions
{

Common::GpsPositionData getGpsPosition() noexcept
{
    return GpsPositionData{
        PositionData{12, 10},
        Timestamp{"00:00:00.000"},
        Date{"01.01.1970"},
        VelocityData{1},
    };
}

std::string getGpsPositionAsJson() noexcept
{
    // clang-format off
    return R"({"latitude":"12","longitude":"10","time":"00:00:00.000","date":"01.01.1970","velocity":1.0})";
    // clang-format on
}

} // namespace Rapid::TestHelper::GpsPositions
