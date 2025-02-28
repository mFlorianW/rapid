// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <testhelper/SessionDatabaseMock.hpp>
#include <testhelper/Sessions.hpp>
#include <workflow/LocalSessionManagement.hpp>

using namespace Rapid::Workflow;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;
using namespace Rapid::Storage;
using namespace trompeloeil;

namespace
{

struct TestFixture
{
    SessionDatabaseMock db;
    std::shared_ptr<GetSessionResult> sessionResult = std::make_shared<GetSessionResult>();
};

} // namespace

TEST_CASE_METHOD(TestFixture, "The LocalSessionManagement shall give session data")
{
    auto const metaData = Sessions::getTestSessionMetaData();
    auto smgmt = LocalSessionManagement{std::addressof(db)};
    REQUIRE_CALL(db, getSessionByMetadataAsync(_)).WITH(_1 == metaData).LR_RETURN(sessionResult);
    auto const session = smgmt.getSession(Sessions::getTestSessionMetaData());
}
