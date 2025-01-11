// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_TESTHELPER_UBLOXDATAPROVIDERMOCK_HPP
#define RAPID_TESTHELPER_UBLOXDATAPROVIDERMOCK_HPP

#include <positioning/IUbloxDevice.hpp>
#include <trompeloeil.hpp>

namespace Rapid::TestHelper
{

class UbloxDeviceMock : public Positioning::IUbloxDevice
{
    MAKE_MOCK(isReady, auto()->bool, const noexcept override);
    MAKE_MOCK(write, auto(std::vector<std::uint8_t> const& data)->void, override);
    MAKE_MOCK(read, auto()->std::vector<std::uint8_t>, override);
    MAKE_MOCK(hasDataPending, auto()->bool, const noexcept);
};

} // namespace Rapid::TestHelper

#endif // !RAPID_TESTHELPER_UBLOXDATAPROVIDERMOCK_HPP
