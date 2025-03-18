// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <testhelper/RestClientMock.hpp>
#include <testhelper/Sessions.hpp>
#include <workflow/qt/RestSessionManagementWorkflow.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::Rest;
using namespace trompeloeil;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;
using namespace Rapid::Common::Qt;

struct TestFixture
{
    Rapid::TestHelper::RestClientMock restClient{};
    RestSessionManagementWorkflow rDl{&restClient};
    std::shared_ptr<SessionMetadataProvider> provider{rDl.getProvider()};
};

TEST_CASE_METHOD(TestFixture, "The RestSessionDownloader shall have a SessionMetadataProvider")
{
    auto restCall = std::make_shared<RestCallMock>();
    auto resultHandler = [&restCall](std::string_view path) {
        if (path == "/sessions/0/metadata") {
            restCall->setData(Sessions::getTestSessionMetaAsJson());
        } else {
            restCall->setData(Sessions::getTestSessionMetadataAsJson2());
        }
        restCall->setCallResult(RestCallResult::Success);
    };

    SECTION("On creation the SessionMetadataProvider is empty")
    {
        REQUIRE(provider->getRowCount() == 0);
    }

    SECTION("On finished session meta download insert the SessionMetadataProvider")
    {
        REQUIRE_CALL(restClient, execute(_))
            .TIMES(2)
            .WITH(_1.getType() == RequestType::Get)
            .WITH(_1.getPath() == Path("/sessions/0/metadata") or _1.getPath() == Path("/sessions/1/metadata"))
            .SIDE_EFFECT(resultHandler(_1.getPath().getPath()))
            .LR_RETURN(restCall);

        rDl.downloadSessionMetadata(0);

        CHECK(provider->getRowCount() == 1);
        CHECK(provider->getItem(0).value_or(Rapid::Common::SessionData{}) == Sessions::getTestSessionMetaData());

        rDl.downloadSessionMetadata(1);

        CHECK(provider->getRowCount() == 2);
        CHECK(provider->getItem(1).value_or(Rapid::Common::SessionData{}) == Sessions::getTestSessionMetaData2());
    }

    SECTION("On finished session meta download the data is updated")
    {
        REQUIRE_CALL(restClient, execute(_))
            .TIMES(2)
            .WITH(_1.getType() == RequestType::Get)
            .WITH(_1.getPath() == Path("/sessions/0/metadata"))
            .SIDE_EFFECT(resultHandler(_1.getPath().getPath()))
            .LR_RETURN(restCall);

        rDl.downloadSessionMetadata(0);
        CHECK(provider->getRowCount() == 1);
        CHECK(provider->getItem(0).value_or(Rapid::Common::SessionData{}) == Sessions::getTestSessionMetaData());

        rDl.downloadSessionMetadata(0);
        CHECK(provider->getRowCount() == 1);
        CHECK(provider->getItem(0).value_or(Rapid::Common::SessionData{}) == Sessions::getTestSessionMetaData());
    }
}
