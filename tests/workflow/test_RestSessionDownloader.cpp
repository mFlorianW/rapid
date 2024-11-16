// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestSessionDownloader.hpp"
#include "RestSessionDownloaderClient.hpp"
#include "Sessions.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Workflow;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;

SCENARIO("The RestSessionDownload shall fetch the stored session count on the laptimer")
{
    GIVEN("A setuped RestSessionDownload")
    {
        auto restClient = RestSessionDownloaderClient{};
        auto rDl = RestSessionDownloader{restClient};
        auto fetchSignalEmitted = false;
        auto fetchStatus = DownloadResult::Error;
        rDl.sessionCountFetched.connect([&](DownloadResult result) {
            fetchSignalEmitted = true;
            fetchStatus = DownloadResult::Ok;
        });

        WHEN("Fetching the SessionCount on the device")
        {
            rDl.fetchSessionCount();
            THEN("Then the correct session count shall be returned.")
            {
                constexpr auto sessionCount = std::size_t{2};
                REQUIRE(fetchSignalEmitted);
                REQUIRE(fetchStatus == DownloadResult::Ok);
                REQUIRE(rDl.getSessionCount() == sessionCount);
            }
        }
    }
}

SCENARIO("The RestSessionDownload shall download a specific session stored on the device")
{
    GIVEN("A setuped RestSessionDownloader")
    {
        auto restClient = RestSessionDownloaderClient{};
        auto rDl = RestSessionDownloader{restClient};
        auto sessionDownloadedEmitted = false;
        auto sessionDownloadEmittedIndex = std::size_t{12};
        rDl.sessionDownloadFinshed.connect([&](std::size_t index, DownloadResult result) {
            sessionDownloadEmittedIndex = index;
            sessionDownloadedEmitted = true;
            REQUIRE(result == DownloadResult::Ok);
        });

        WHEN("Downloading the session with index 0")
        {
            rDl.downloadSession(0);
            THEN("Then correct session shall be downloaded")
            {
                REQUIRE(sessionDownloadedEmitted);
                REQUIRE(sessionDownloadEmittedIndex == 0);
                REQUIRE(rDl.getSession(0).value_or(SessionData{}) == Sessions::getTestSession());
            }
        }
    }
}
