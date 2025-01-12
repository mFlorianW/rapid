// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef POSITIONS_HPP
#define POSITIONS_HPP

#include "common/PositionData.hpp"

namespace Rapid::TestHelper::Positions
{
Common::PositionData getOscherslebenPositionCamp();
Common::PositionData getOscherslebenPositionStartFinishLine();
Common::PositionData getOscherslebenPositionSector1Line();
Common::PositionData getOscherslebenPositionSector2Line();

Common::PositionData getOscherslebenStartFinishLine1();
Common::PositionData getOscherslebenStartFinishLine2();
Common::PositionData getOscherslebenStartFinishLine3();
Common::PositionData getOscherslebenStartFinishLine4();

Common::PositionData getOscherslebenSector1Point1();
Common::PositionData getOscherslebenSector1Point2();
Common::PositionData getOscherslebenSector1Point3();
Common::PositionData getOscherslebenSector1Point4();

Common::PositionData getOscherslebenSector2Point1();
Common::PositionData getOscherslebenSector2Point2();
Common::PositionData getOscherslebenSector2Point3();
Common::PositionData getOscherslebenSector2Point4();
} // namespace Rapid::TestHelper::Positions

#endif //! POSITIONS_HPP
