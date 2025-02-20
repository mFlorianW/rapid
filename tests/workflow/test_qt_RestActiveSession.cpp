// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QSignalSpy>
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <testhelper/RestClientMock.hpp>
#include <testhelper/Tracks.hpp>
#include <workflow/qt/RestActiveSession.hpp>

using namespace Rapid::TestHelper;
using namespace Rapid::Rest;
using namespace trompeloeil;

namespace
{

struct TestFixture
{
    RestClientMock restClientMock;
    std::shared_ptr<RestCallMock> restCall = std::make_shared<RestCallMock>();
    Rapid::Workflow::Qt::RestActiveSession activeSession{std::addressof(restClientMock)};
};

} // namespace

TEST_CASE_METHOD(TestFixture, "The RestActiveSession shall provide track data")
{
    activeSession.track.set(Tracks::getOscherslebenTrack());
    SECTION("Has a property with the name \"trackName\"")
    {
        REQUIRE(activeSession.property("trackName").value<QString>() == QStringLiteral("Oschersleben"));
    }

    SECTION("The track name property is updated when track property changes")
    {
        auto changedSpy =
            QSignalSpy{std::addressof(activeSession), &Rapid::Workflow::Qt::RestActiveSession::trackNameChanged};
        activeSession.track.set(Tracks::getTrack());
        REQUIRE(changedSpy.size() == 1);
        REQUIRE(activeSession.getTrackName() == QStringLiteral("Track"));
    }
}

TEST_CASE_METHOD(TestFixture, "The RestActiveSession shall provide lap count")
{
    activeSession.lapCount.set(10);
    SECTION("Has a property with the name \"lapCount\"")
    {
        REQUIRE(activeSession.property("lapCount").value<quint32>() == 10);
    }

    SECTION("The lap count property is updated when lap data is updated")
    {
        auto changedSpy =
            QSignalSpy{std::addressof(activeSession), &Rapid::Workflow::Qt::RestActiveSession::lapCountChanged};
        activeSession.lapCount.set(12);
        REQUIRE(changedSpy.size() == 1);
        REQUIRE(activeSession.getLapCount() == 12);
    }
}

TEST_CASE_METHOD(TestFixture, "The RestActiveSession shall provide current lap time")
{
    activeSession.currentLap.set(Rapid::Common::Timestamp{"00:00:01.000"});
    SECTION("Has a property with the name \"currentLapTime\"")
    {
        REQUIRE(activeSession.property("currentLapTime").value<QString>() == QStringLiteral("00:00:01.000"));
    }

    SECTION("The lap count property is updated when lap data is updated")
    {
        auto const lapTime = Rapid::Common::Timestamp{"00:00:02.000"};
        auto changedSpy =
            QSignalSpy{std::addressof(activeSession), &Rapid::Workflow::Qt::RestActiveSession::currentLapTimeChanged};
        activeSession.currentLap.set(lapTime);
        REQUIRE(changedSpy.size() == 1);
        REQUIRE(activeSession.getCurrentLapTime() == QString::fromStdString(lapTime.asString()));
    }
}

TEST_CASE_METHOD(TestFixture, "The RestActiveSession shall provide current sector time")
{
    activeSession.currentSector.set(Rapid::Common::Timestamp{"00:00:01.000"});
    SECTION("Has a property with the name \"currentSectorTime\"")
    {
        REQUIRE(activeSession.property("currentSectorTime").value<QString>() == QStringLiteral("00:00:01.000"));
    }

    SECTION("The lap count property is updated when lap data is updated")
    {
        auto const lapTime = Rapid::Common::Timestamp{"00:00:02.000"};
        auto changedSpy = QSignalSpy{std::addressof(activeSession),
                                     &Rapid::Workflow::Qt::RestActiveSession::currentSectorTimeChanged};
        activeSession.currentSector.set(lapTime);
        REQUIRE(changedSpy.size() == 1);
        REQUIRE(activeSession.getCurrentSectorTime() == QString::fromStdString(lapTime.asString()));
    }
}

TEST_CASE_METHOD(TestFixture, "The RestActiveSession shall update the track data")
{
    auto expectedReq = RestRequest{RequestType::Get, "/activeSession/track"};
    REQUIRE_CALL(restClientMock, execute(_)).LR_WITH(_1 == expectedReq).LR_RETURN(restCall);
    activeSession.updateTrackData();
}

TEST_CASE_METHOD(TestFixture, "The RestActiveSession shall update the lap data")
{
    auto expectedReq = RestRequest{RequestType::Get, "/activeSession/lap"};
    REQUIRE_CALL(restClientMock, execute(_)).LR_WITH(_1 == expectedReq).LR_RETURN(restCall);
    activeSession.updateLapData();
}
