// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>
#include <common/qt/SessionMetadataProvider.hpp>
#include <testhelper/Sessions.hpp>

using namespace Rapid::Common::Qt;
using namespace Rapid::TestHelper;

struct TestFixture
{
    SessionMetadataProvider sessionMetaDataProvider;
};

TEST_CASE_METHOD(TestFixture, "The SessionMetaDataProvider shall set correct columnames")
{
    CHECK(sessionMetaDataProvider.getColumnCount() == 3);
    REQUIRE(sessionMetaDataProvider.getColumnNames() ==
            QStringList{QStringLiteral("Date"), QStringLiteral("Time"), QStringLiteral("Track")});
}

TEST_CASE_METHOD(TestFixture, "The SessionMetaDataProvider shall give the correct SessionMetaData")
{
    sessionMetaDataProvider.addItem(Sessions::getTestSessionMetaData());
    sessionMetaDataProvider.addItem(Sessions::getTestSessionMetaData());

    CHECK(sessionMetaDataProvider.data(0, 0, ::Qt::DisplayRole).toString() == QStringLiteral("01.01.1970"));
    CHECK(sessionMetaDataProvider.data(0, 1, ::Qt::DisplayRole).toString() == QStringLiteral("13:00:00.000"));
    CHECK(sessionMetaDataProvider.data(0, 2, ::Qt::DisplayRole).toString() == QStringLiteral("Track"));
    CHECK(sessionMetaDataProvider.data(1, 0, ::Qt::DisplayRole).toString() == QStringLiteral("01.01.1970"));
    CHECK(sessionMetaDataProvider.data(1, 1, ::Qt::DisplayRole).toString() == QStringLiteral("13:00:00.000"));
    CHECK(sessionMetaDataProvider.data(1, 2, ::Qt::DisplayRole).toString() == QStringLiteral("Track"));
}
