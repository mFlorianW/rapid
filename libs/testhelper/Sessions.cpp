// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Sessions.hpp"
#include "Tracks.hpp"
#include <common/GpsPositionData.hpp>

using namespace Rapid::Common;

namespace Rapid::TestHelper::Sessions
{

SessionData createSession()
{
    Date sessionDate;
    sessionDate.setYear(1970);
    sessionDate.setMonth(1);
    sessionDate.setDay(1);
    Timestamp sessionTime;
    sessionTime.setHour(13);

    Timestamp sectorTime;
    sectorTime.setFractionalOfSecond(144);
    sectorTime.setSecond(25);

    auto gpsPos =
        GpsPositionData{PositionData{52.00f, 11.00f}, Timestamp{"00:00:00.000"}, Date{"01.01.1970"}, VelocityData{100}};

    LapData lap;
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addPosition(gpsPos);
    lap.addPosition(gpsPos);

    auto session = SessionData{Tracks::getOscherslebenTrack(), sessionDate, sessionTime, 10};
    session.addLap(lap);

    return session;
}

SessionData createSession2()
{
    Date sessionDate;
    sessionDate.setYear(1970);
    sessionDate.setMonth(2);
    sessionDate.setDay(1);
    Timestamp sessionTime;
    sessionTime.setHour(13);

    Timestamp sectorTime;
    sectorTime.setFractionalOfSecond(144);
    sectorTime.setSecond(25);

    LapData lap;
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);

    auto session = SessionData{Tracks::getOscherslebenTrack(), sessionDate, sessionTime};
    session.addLap(lap);

    return session;
}

SessionData createSession3()
{
    Date sessionDate;
    sessionDate.setYear(1970);
    sessionDate.setMonth(2);
    sessionDate.setDay(1);
    Timestamp sessionTime;
    sessionTime.setHour(13);

    Timestamp sectorTime;
    sectorTime.setFractionalOfSecond(144);
    sectorTime.setSecond(25);

    auto gpsPos = GpsPositionData{PositionData{52.0258333, 11.279166666},
                                  Timestamp{"00:00:00.000"},
                                  Date{"01.01.1970"},
                                  VelocityData{100}};

    LapData lap;
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addPosition(gpsPos);
    lap.addPosition(gpsPos);

    auto session = SessionData{Tracks::getOscherslebenTrack2(), sessionDate, sessionTime, 10};
    session.addLap(lap);
    session.addLap(lap);

    return session;
}

SessionData createSession4()
{
    Date sessionDate;
    sessionDate.setYear(1970);
    sessionDate.setMonth(2);
    sessionDate.setDay(1);
    Timestamp sessionTime;
    sessionTime.setHour(15);

    Timestamp sectorTime;
    sectorTime.setFractionalOfSecond(144);
    sectorTime.setSecond(25);

    LapData lap;
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);
    lap.addSectorTime(sectorTime);

    auto session = SessionData{Tracks::getOscherslebenTrack2(), sessionDate, sessionTime};
    session.addLap(lap);
    session.addLap(lap);

    return session;
}

SessionData getTestSession3()
{
    return createSession3();
}

SessionData getTestSession()
{
    return createSession();
}

SessionData getTestSession2()
{
    return createSession2();
}

char const* getTestSessionAsJson()
{
    // clang-format off
    static constexpr const char* TestSessionAsJson = {
        "{"
            "\"id\":10,"
            "\"date\":\"01.01.1970\","
            "\"time\":\"13:00:00.000\","
            "\"track\":{"
                "\"name\":\"Oschersleben\","
                "\"startline\":{"
                    "\"latitude\":\"52.025833\","
                    "\"longitude\":\"11.279166\""
                "},"
            "\"finishline\":{"
                    "\"latitude\":\"52.025833\","
                    "\"longitude\":\"11.279166\""
            "},"
            "\"sectors\":["
                "{"
                    "\"latitude\":\"52.025833\","
                    "\"longitude\":\"11.279166\""
                "},"
                "{"
                    "\"latitude\":\"52.025833\","
                    "\"longitude\":\"11.279166\""
                "}"
            "]"
            "},"
            "\"laps\":["
                "{"
                    "\"sectors\":["
                            "\"00:00:25.144\","
                            "\"00:00:25.144\","
                            "\"00:00:25.144\","
                            "\"00:00:25.144\""
                        "],"
                    "\"log_points\":["
                        "{"
                            "\"velocity\":100.0,"
                            "\"longitude\":11.0,"
                            "\"latitude\":52.0,"
                            "\"time\":\"00:00:00.000\","
                            "\"date\":\"01.01.1970\""
                        "},"
                        "{"
                            "\"velocity\":100.0,"
                            "\"longitude\":11.0,"
                            "\"latitude\":52.0,"
                            "\"time\":\"00:00:00.000\","
                            "\"date\":\"01.01.1970\""
                        "}"
                    "]"
                "}"
            "]"
        "}"
    };
    // clang-format on
    return TestSessionAsJson;
}

SessionData getTestSession4()
{
    return createSession4();
}

Common::SessionMetaData getTestSessionMetaData()
{
    Date sessionDate;
    sessionDate.setYear(1970);
    sessionDate.setMonth(1);
    sessionDate.setDay(1);
    Timestamp sessionTime;
    sessionTime.setHour(13);
    return SessionMetaData{Tracks::getTrack(), sessionDate, sessionTime, 10};
}

Common::SessionMetaData getTestSessionMetaData2()
{
    Date sessionDate;
    sessionDate.setYear(1970);
    sessionDate.setMonth(2);
    sessionDate.setDay(1);
    Timestamp sessionTime;
    sessionTime.setHour(13);
    return SessionMetaData{Tracks::getTrack(), sessionDate, sessionTime, 10};
}

char const* getTestSessionMetaAsJson()
{
    // clang-format off
    static constexpr const char* TestSessionAsJson = {
        "{"
            "\"id\":10,"
            "\"date\":\"01.01.1970\","
            "\"time\":\"13:00:00.000\","
            "\"track\":{"
                "\"name\":\"Track\","
                "\"startline\":{"
                    "\"latitude\":\"52\","
                    "\"longitude\":\"11.2\""
                "},"
                "\"finishline\":{"
                        "\"latitude\":\"52\","
                        "\"longitude\":\"11.2\""
                "},"
                "\"sectors\":["
                    "{"
                        "\"latitude\":\"52\","
                        "\"longitude\":\"11.2\""
                    "},"
                    "{"
                        "\"latitude\":\"52\","
                        "\"longitude\":\"11.2\""
                    "}"
                "]"
            "}"
        "}"
    };
    return TestSessionAsJson;
}

const char* getTestSessionMetadataAsJson2()
{
    // clang-format off
    static constexpr const char* TestSessionAsJson = {
        "{"
            "\"id\":10,"
            "\"date\":\"01.02.1970\","
            "\"time\":\"13:00:00.000\","
            "\"track\":{"
                "\"name\":\"Track\","
                "\"startline\":{"
                    "\"latitude\":\"52\","
                    "\"longitude\":\"11.2\""
                "},"
                "\"finishline\":{"
                        "\"latitude\":\"52\","
                        "\"longitude\":\"11.2\""
                "},"
                "\"sectors\":["
                    "{"
                        "\"latitude\":\"52\","
                        "\"longitude\":\"11.2\""
                    "},"
                    "{"
                        "\"latitude\":\"52\","
                        "\"longitude\":\"11.2\""
                    "}"
                "]"
            "}"
        "}"
    };
    // clang-format on
    return TestSessionAsJson;
}

} // namespace Rapid::TestHelper::Sessions
