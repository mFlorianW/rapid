// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "testhelper/Laptimer.hpp"
#include "testhelper/PositionDateTimeProvider.hpp"
#include "testhelper/Positions.hpp"
#include "testhelper/SessionDatabaseMock.hpp"
#include "testhelper/SignalSpy.hpp"
#include "workflow/ActiveSessionWorkflow.hpp"

using namespace Rapid::Workflow;
using namespace Rapid::TestHelper;
using namespace Rapid::Storage;
using namespace Rapid::Common;

class TestFixture
{
public:
    Laptimer lp{};
    PositionDateTimeProvider dp{};
    SessionDatabaseMock sdb{};
    ActiveSessionWorkflow actSessWf{dp, lp, sdb};
};

TEST_CASE_METHOD(TestFixture, "The ActiveSessionWorkflow shall be able to start and stop", "[ACTIVESESSION_WORKFLOW]")
{
    SECTION("Shall not do return a session when start isn't called")
    {
        REQUIRE_FALSE(actSessWf.getSession());
    }

    SECTION("Shall return a session when start called")
    {
        actSessWf.startActiveSession();
        REQUIRE(actSessWf.getSession());
    }

    SECTION("The ActiveSessionWorkflow shall not return a valid session when stop is called.")
    {
        actSessWf.startActiveSession();
        REQUIRE(actSessWf.getSession());
        actSessWf.stopActiveSession();
        REQUIRE(!actSessWf.getSession());
    }
}

TEST_CASE_METHOD(TestFixture,
                 "The ActiveSessionWorkflow shall session data in the database",
                 "[ACTIVESESSION_WORKFLOW]")
{
    SECTION("Store session data when the laptimer emits lap finished")
    {
        auto expectedLap = LapData{Timestamp{"00:23:13.123"}};
        auto res = std::make_shared<Rapid::System::AsyncResult>();
        res->setResult(Rapid::System::Result::Ok);

        REQUIRE_CALL(sdb, storeSession(trompeloeil::_)).WITH(_1.getNumberOfLaps() == 1).RETURN(res);

        actSessWf.startActiveSession();
        lp.sectorTimes.emplace_back("00:23:13.123");
        lp.lapFinished.emit();

        REQUIRE(actSessWf.getSession().value_or(SessionData{}).getLaps().size() == 1);
        REQUIRE(actSessWf.getSession().value_or(SessionData{}).getLap(0) == expectedLap);
    }

    SECTION("Store the sector times in a lap of a session")
    {
        auto expectedLap = LapData{{
            Timestamp{"00:23:123.233"},
            Timestamp{"00:23:123.233"},
        }};

        REQUIRE_CALL(sdb, storeSession(trompeloeil::_)).RETURN(std::make_shared<Rapid::System::AsyncResult>());

        actSessWf.startActiveSession();

        lp.sectorTimes.emplace_back("00:23:123.233");
        lp.sectorFinished.emit();

        lp.sectorTimes.emplace_back("00:23:123.233");
        lp.lapFinished.emit();

        REQUIRE(actSessWf.getSession());
        REQUIRE(actSessWf.getSession().value_or(SessionData{}).getNumberOfLaps() == 1);
        REQUIRE(actSessWf.getSession().value_or(SessionData{}).getLap(0) == expectedLap);
    }

    SECTION("Store every GPS position updates to the lap when the lap is started")
    {
        auto const expectedPos =
            GpsPositionData{PositionData{52.1, 11.3}, Timestamp{"00:00:00.000"}, Date{"01.01.1970"}, VelocityData{100}};

        ALLOW_CALL(sdb, storeSession(trompeloeil::_)).RETURN(std::make_shared<Rapid::System::AsyncResult>());

        actSessWf.startActiveSession();
        lp.lapStarted.emit();
        dp.gpsPosition.set(expectedPos);
        lp.lapFinished.emit();

        auto const session = actSessWf.getSession();

        // NOLINTBEGIN(bugprone-unchecked-optional-access)
        REQUIRE(session->getNumberOfLaps() == 1);
        REQUIRE(session->getLap(0)->getPositions().size() == 1);
        REQUIRE(session->getLap(0)->getPositions().at(0) == expectedPos);
        // NOLINTEND(bugprone-unchecked-optional-access)
    }
}

TEST_CASE_METHOD(TestFixture, "The ActiveSessionWorkflow shall emit finished signals", "[ACTIVESESSION_WORKFLOW]")
{
    SECTION("Emit the lap finished signal.")
    {
        auto expLapTimer = std::string{"00:23:13.123"};
        auto lapFinishedSpy = SignalSpy{actSessWf.lapFinished};

        ALLOW_CALL(sdb, storeSession(trompeloeil::_)).RETURN(std::make_shared<Rapid::System::AsyncResult>());

        actSessWf.startActiveSession();
        lp.sectorTimes.emplace_back(expLapTimer);
        lp.lapFinished.emit();

        REQUIRE(lapFinishedSpy.getCount() == 1);
    }

    SECTION("Emits the sector finished signal.")
    {
        auto expectedSectorTime = Timestamp{"00:00:12.123"};
        auto sectorTimeSpy = SignalSpy{actSessWf.sectorFinshed};

        actSessWf.startActiveSession();
        lp.sectorTimes.emplace_back("00:00:12.123");
        lp.sectorFinished.emit();

        REQUIRE(sectorTimeSpy.getCount() == 1);
    }
}

TEST_CASE_METHOD(TestFixture, "The ActiveSessionWorkflow shall update the properties", "[ACTIVESESSION_WORKFLOW]")
{
    SECTION("Update the round laptime property when the laptimer notifes that the lap is finished")
    {
        auto expectedLaptime = Timestamp{"00:00:12.123"};

        ALLOW_CALL(sdb, storeSession(trompeloeil::_)).RETURN(std::make_shared<Rapid::System::AsyncResult>());

        actSessWf.startActiveSession();
        lp.sectorTimes.emplace_back("00:00:12.123");
        lp.lapFinished.emit();

        REQUIRE(actSessWf.lastLaptime.get() == expectedLaptime);
    }

    SECTION("Update the currentLaptime property when the laptimer update the current laptime")
    {
        actSessWf.startActiveSession();

        lp.currentLaptime.set(Timestamp{"00:00:30.123"});
        REQUIRE(actSessWf.currentLaptime.get() == Timestamp{"00:00:30.123"});

        lp.currentLaptime.set(Timestamp{"00:00:45.123"});
        REQUIRE(actSessWf.currentLaptime.get() == Timestamp{"00:00:45.123"});

        lp.currentLaptime.set(Timestamp{"00:01:15.123"});
        REQUIRE(actSessWf.currentLaptime.get() == Timestamp{"00:01:15.123"});
    }

    SECTION("Update the currentSectorTime property when the laptimer update the current sectorTime.")
    {
        actSessWf.startActiveSession();

        lp.currentSectorTime.set(Timestamp{"00:00:30.123"});
        REQUIRE(actSessWf.currentSectorTime.get() == Timestamp{"00:00:30.123"});

        lp.currentSectorTime.set(Timestamp{"00:00:45.123"});
        REQUIRE(actSessWf.currentSectorTime.get() == Timestamp{"00:00:45.123"});

        lp.currentSectorTime.set(Timestamp{"00:01:15.123"});
        REQUIRE(actSessWf.currentSectorTime.get() == Timestamp{"00:01:15.123"});
    }

    SECTION("Update the lap counter when a lap is finished.")
    {
        ALLOW_CALL(sdb, storeSession(trompeloeil::_)).RETURN(std::make_shared<Rapid::System::AsyncResult>());

        actSessWf.startActiveSession();

        REQUIRE(actSessWf.lapCount.get() == 0);

        lp.sectorTimes.emplace_back("00:23:32.123");
        lp.lapFinished.emit();

        REQUIRE(actSessWf.lapCount.get() == 1);

        lp.lapFinished.emit();
        REQUIRE(actSessWf.lapCount.get() == 2);
    }
}

TEST_CASE_METHOD(TestFixture,
                 "The ActiveSessionWorkflow shall forward GPS position updates",
                 "[ACTIVESESSION_WORKFLOW]")
{
    SECTION("Forward all GPS position updates to the laptimer when session is active")
    {
        actSessWf.startActiveSession();
        dp.gpsPosition.set(GpsPositionData{Positions::getOscherslebenPositionStartFinishLine(), {}, {}});

        REQUIRE(lp.lastPostionDateTime == GpsPositionData{Positions::getOscherslebenPositionStartFinishLine(), {}, {}});
    }

    SECTION("Forward all PositionTimeDate updates to the laptimer when session is stopped")
    {
        actSessWf.startActiveSession();
        actSessWf.stopActiveSession();
        dp.gpsPosition.set(GpsPositionData{Positions::getOscherslebenPositionStartFinishLine(), {}, {}});

        REQUIRE(lp.lastPostionDateTime == GpsPositionData{{}, {}, {}});
    }
}
