// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <SessionAnalyzeWorkflow.hpp>
#include <Sessions.hpp>
#include <catch2/catch_all.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::TestHelper;

TEST_CASE("The SessionAnalyzeWorkflow shall create a LapDataModel when a session is set")
{
    auto sa = SessionAnalyzeWorkflow();
    sa.setSession(Sessions::getTestSession3());
    REQUIRE(sa.lapModel.get()->rowCount() == 2);
}
