// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <testhelper/Sessions.hpp>
#include <workflow/qt/SessionAnalyzeWorkflow.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::TestHelper;

TEST_CASE("The SessionAnalyzeWorkflow shall create a LapDataModel when a session is set")
{
    auto sa = SessionAnalyzeWorkflow();
    sa.setSession(Sessions::getTestSession3());
    REQUIRE(sa.lapModel.get()->rowCount() == 2);
}
