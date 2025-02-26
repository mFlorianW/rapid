// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <testhelper/RestClientMock.hpp>
#include <testhelper/SessionDatabaseMock.hpp>
#include <testhelper/Sessions.hpp>
#include <workflow/qt/LocalSessionManagement.hpp>

using namespace Rapid::TestHelper;
using namespace Rapid::Workflow::Qt;
using namespace Rapid::Common::Qt;
using namespace Rapid::Storage;
using namespace trompeloeil;

namespace
{

struct TestFixture
{
    SessionDatabaseMock db;
    std::shared_ptr<GetSessionMetaDataResult> metaDataResult = std::make_shared<GetSessionMetaDataResult>();
};

} // namespace

TEST_CASE_METHOD(TestFixture,
                 "The LocalSessionManagement shall provide a list model with local stored sessions",
                 "[LocalSessionManagementModel]")
{
    ALLOW_CALL(db, getSessionCount()).RETURN(std::size_t{1});
    ALLOW_CALL(db, getSessionMetaDataByIndexAsync(_))
        .LR_SIDE_EFFECT(metaDataResult->setResultValue(Sessions::getTestSessionMetaData()))
        .LR_SIDE_EFFECT(metaDataResult->setResult(Rapid::System::Result::Ok))
        .LR_RETURN(metaDataResult);
    auto smgmt = LocalSessionManagement{std::addressof(db)};

    SECTION("The list model property returns a valid value")
    {
        REQUIRE(smgmt.property("listModel").value<SessionMetaDataListModel*>() != nullptr);
    }

    SECTION("The list model contains the sessions stored in the session database")
    {
        REQUIRE(smgmt.getSessionMetaDataListModel()->rowCount({}) == 1);
    }
}
