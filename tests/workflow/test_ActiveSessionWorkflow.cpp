// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ActiveSessionWorkflow.hpp"
#include "Laptimer.hpp"
#include "MemorySessionDatabaseBackend.hpp"
#include "PositionDateTimeProvider.hpp"
#include "Positions.hpp"
#include "SessionDatabase.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Workflow;
using namespace Rapid::TestHelper;
using namespace Rapid::Storage;
using namespace Rapid::Common;

TEST_CASE("The ActiveSessionWorkflow shall not do return a session when start isn't called")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    REQUIRE(!actSessWf.getSession());
}

TEST_CASE("The ActiveSessionWorkflow shall return a session when start called")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();

    REQUIRE(actSessWf.getSession());
}

TEST_CASE("The ActiveSessionWorkflow shall store a lap in the database when the laptimer emits lap finished")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();
    lp.lapFinished.emit();

    REQUIRE(sdb.getSessionCount() == 1);
}

TEST_CASE("The ActiveSessionWorkflow shall not return a valid session when stop is called.")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();

    REQUIRE(actSessWf.getSession());

    actSessWf.stopActiveSession();

    REQUIRE(!actSessWf.getSession());
}

TEST_CASE("The ActiveSessionWorkflow shall update the round sector time property when the laptimer notifes that sector "
          "is finished.")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};
    auto expectedSectorTime = Timestamp{"00:00:12.123"};
    bool sectorFinishedEmitted = false;

    actSessWf.sectorFinshed.connect([&sectorFinishedEmitted]() {
        sectorFinishedEmitted = true;
    });
    actSessWf.startActiveSession();
    lp.sectorTimes.emplace_back("00:00:12.123");
    lp.sectorFinished.emit();

    REQUIRE(actSessWf.lastSectorTime.get() == expectedSectorTime);
    REQUIRE(sectorFinishedEmitted == true);
}

TEST_CASE("The ActiveSessionWorkflow shall store the laptime when finished.")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};
    auto expectedLap = LapData{Timestamp{"00:23:13.123"}};
    bool lapFinishedEmitted = false;

    actSessWf.startActiveSession();
    actSessWf.lapFinished.connect([&lapFinishedEmitted]() {
        lapFinishedEmitted = true;
    });
    lp.sectorTimes.emplace_back("00:23:13.123");
    lp.lapFinished.emit();

    REQUIRE(actSessWf.getSession().value_or(SessionData{}).getLaps().size() == 1);
    REQUIRE(actSessWf.getSession().value_or(SessionData{}).getLap(0) == expectedLap);
    REQUIRE(lapFinishedEmitted == true);
}

TEST_CASE("The ActiveSessionWorkflow shall update the round laptime property when the laptimer notifes that the "
          "is finished.")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};
    auto expectedLaptime = Timestamp{"00:00:12.123"};

    actSessWf.startActiveSession();
    lp.sectorTimes.emplace_back("00:00:12.123");
    lp.lapFinished.emit();

    REQUIRE(actSessWf.lastLaptime.get() == expectedLaptime);
}

TEST_CASE(
    "The ActiveSessionWorkflow shall update the currentLaptime property when the laptimer update the current laptime.")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();

    lp.currentLaptime.set(Timestamp{"00:00:30.123"});
    REQUIRE(actSessWf.currentLaptime.get() == Timestamp{"00:00:30.123"});

    lp.currentLaptime.set(Timestamp{"00:00:45.123"});
    REQUIRE(actSessWf.currentLaptime.get() == Timestamp{"00:00:45.123"});

    lp.currentLaptime.set(Timestamp{"00:01:15.123"});
    REQUIRE(actSessWf.currentLaptime.get() == Timestamp{"00:01:15.123"});
}

TEST_CASE("The ActiveSessionWorkflow shall update the currentSectorTime property when the laptimer update the current "
          "sectorTime.")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();

    lp.currentSectorTime.set(Timestamp{"00:00:30.123"});
    REQUIRE(actSessWf.currentSectorTime.get() == Timestamp{"00:00:30.123"});

    lp.currentSectorTime.set(Timestamp{"00:00:45.123"});
    REQUIRE(actSessWf.currentSectorTime.get() == Timestamp{"00:00:45.123"});

    lp.currentSectorTime.set(Timestamp{"00:01:15.123"});
    REQUIRE(actSessWf.currentSectorTime.get() == Timestamp{"00:01:15.123"});
}

TEST_CASE("The ActiveSessionWorkflow shall store the sector times in a lap of a session")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};
    auto expectedLap = LapData{{
        Timestamp{"00:23:123.233"},
        Timestamp{"00:23:123.233"},
    }};

    actSessWf.startActiveSession();

    lp.sectorTimes.emplace_back("00:23:123.233");
    lp.sectorFinished.emit();

    lp.sectorTimes.emplace_back("00:23:123.233");
    lp.lapFinished.emit();

    REQUIRE(actSessWf.getSession());
    REQUIRE(actSessWf.getSession().value_or(SessionData{}).getNumberOfLaps() == 1);
    REQUIRE(actSessWf.getSession().value_or(SessionData{}).getLap(0) == expectedLap);
}

TEST_CASE("The ActiveSessionWorkflow shall update the lap counter when a lap is finished.")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();

    REQUIRE(actSessWf.lapCount.get() == 0);

    lp.sectorTimes.emplace_back("00:23:32.123");
    lp.lapFinished.emit();

    REQUIRE(actSessWf.lapCount.get() == 1);

    lp.lapFinished.emit();
    REQUIRE(actSessWf.lapCount.get() == 2);
}

TEST_CASE("The ActiveSessionWorkflow shall forward all PositionTimeDate Updates to the laptimer when session is active")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();
    dp.positionTimeData.set(PositionDateTimeData{Positions::getOscherslebenPositionStartFinishLine(), {}, {}});

    REQUIRE(lp.lastPostionDateTime ==
            PositionDateTimeData{Positions::getOscherslebenPositionStartFinishLine(), {}, {}});
}

TEST_CASE("The ActiveSessionWorkflow shall not forward all PositionTimeDate updates to the laptimer when session is "
          "stopped")
{
    auto lp = Laptimer{};
    auto dp = PositionDateTimeProvider{};
    auto dbb = MemorySessionDatabaseBackend{};
    auto sdb = SessionDatabase{dbb};
    auto actSessWf = ActiveSessionWorkflow{dp, lp, sdb};

    actSessWf.startActiveSession();
    actSessWf.stopActiveSession();
    dp.positionTimeData.set(PositionDateTimeData{Positions::getOscherslebenPositionStartFinishLine(), {}, {}});

    REQUIRE(lp.lastPostionDateTime == PositionDateTimeData{{}, {}, {}});
}
