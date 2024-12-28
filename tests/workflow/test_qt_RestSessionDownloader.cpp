// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <testhelper/RestClientMock.hpp>
#include <testhelper/Sessions.hpp>
#include <workflow/qt/RestSessionDownloader.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::Rest;
using namespace trompeloeil;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;
using namespace Rapid::Common::Qt;

struct TestFixture
{
    Rapid::TestHelper::RestClientMock restClient{};
    RestSessionDownloader rDl{restClient};
    std::shared_ptr<SessionMetadataProvider> provider{rDl.getProvider()};
};

TEST_CASE_METHOD(TestFixture, "The RestSessionDownloader shall have a SessionMetadataProvider")
{
    SECTION("On creation the SessionMetadataProvider is empty")
    {
        REQUIRE(provider->getRowCount() == 0);
    }

    SECTION("On finished session meta download update the SessionMetadataProvider")
    {
        auto restCall = std::make_shared<RestCallMock>();
        restCall->setData(Sessions::getTestSessionMetaAsJson());

        REQUIRE_CALL(restClient, execute(_))
            .WITH(_1.getType() == RequestType::Get and _1.getPath() == Path("/sessions/0/metadata"))
            .SIDE_EFFECT(restCall->setCallResult(RestCallResult::Success))
            .LR_RETURN(restCall);

        rDl.downloadSessionMetadata(0);

        CHECK(provider->getRowCount() == 1);
        CHECK(provider->getItem(0).has_value());
    }
}
