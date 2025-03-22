// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <positioning/IGPSInformationProvider.hpp>
#include <trompeloeil.hpp>

namespace Rapid::TestHelper
{

class GpsPositionInformationProvider : public Positioning::IGpsInformationProvider
{
public:
    MAKE_MOCK(getGpsFixMode, auto()->Positioning::GpsFixMode, const noexcept override);
    MAKE_MOCK(getNumbersOfStatelite, auto()->std::uint8_t, const noexcept override);
};

} // namespace Rapid::TestHelper
