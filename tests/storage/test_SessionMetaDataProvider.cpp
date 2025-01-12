// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>
#include <storage/qt/SessionMetaDataProvider.hpp>
#include <testhelper/CompareHelper.hpp>
#include <testhelper/SessionDatabaseMock.hpp>
#include <testhelper/Sessions.hpp>

using namespace Rapid::TestHelper;
using namespace Rapid::Storage;
using namespace Rapid::Storage::Qt;
using namespace trompeloeil;

struct TestFixture
{
    SessionDatabaseMock dbMock;
    std::unique_ptr<SessionMetaDataProvider> sessionMetaDataProvider;

    TestFixture()
    {
        REQUIRE_CALL(dbMock, getSessionCount()).RETURN(2);
        REQUIRE_CALL(dbMock, getSessionMetaDataByIndexAsync(any_of(0, 1)))
            .TIMES(2)
            .LR_RETURN(createSuccessSessionMetaDataResult());
        sessionMetaDataProvider = std::make_unique<SessionMetaDataProvider>(dbMock);
    }

    std::shared_ptr<GetSessionMetaDataResult> createSuccessSessionMetaDataResult() const noexcept
    {
        auto result = std::make_shared<GetSessionMetaDataResult>();
        result->setResultValue(Sessions::getTestSessionMetaData());
        result->setResult(Rapid::System::Result::Ok);
        return result;
    }
};

TEST_CASE_METHOD(TestFixture, "The SessionMetaDataProvider shall load SessionMetaData on creation")
{
    REQUIRE(sessionMetaDataProvider->getRowCount() == 2);
}

TEST_CASE_METHOD(TestFixture, "The SessionMetaDataProvider shall handle new SessionMetaData")
{
    REQUIRE_CALL(dbMock, getSessionMetaDataByIndexAsync(2)).LR_RETURN(createSuccessSessionMetaDataResult());
    dbMock.sessionAdded.emit(2);
    REQUIRE_COMPARE_WITH_TIMEOUT(sessionMetaDataProvider->getRowCount(), 3, std::chrono::milliseconds(1));
}

TEST_CASE_METHOD(TestFixture, "The SessionMetaDataProvider shall handle deletion in the database")
{
    SECTION("Remove deleted session the meta data provider")
    {
    }
}
