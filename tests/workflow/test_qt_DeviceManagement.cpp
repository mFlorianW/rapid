// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <testhelper/qt/SettingsBackendMock.hpp>
#include <workflow/qt/DeviceManagement.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::TestHelper;
using namespace trompeloeil;

namespace
{

struct TestFixtue
{
    SettingsBackendMock settingsBackendMock;
};

} // namespace

TEST_CASE_METHOD(TestFixtue, "The DeviceManagement shall give a Model for displaying the devices")
{
    ALLOW_CALL(settingsBackendMock, getValue(_)).RETURN(QVariant{});
    DeviceManagement deviceManagement{std::addressof(settingsBackendMock)};
    SECTION("Give the model a valid model")
    {
        auto* model = deviceManagement.getModel();
        REQUIRE(model != nullptr);
    }
}
