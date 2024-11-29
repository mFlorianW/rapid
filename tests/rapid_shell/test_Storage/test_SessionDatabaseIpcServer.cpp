// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "TestSessionDatabaseIpcClient.h"
#include <JsonDeserializer.hpp>
#include <JsonSerializer.hpp>
#include <QCoreApplication>
#include <QDeadlineTimer>
#include <QFile>
#include <QSignalSpy>
#include <QTest>
#include <QtTestHelper.hpp>
#include <RapidApplication.hpp>
#include <SessionDatabaseIpcServer.hpp>
#include <SessionDatabaseMock.hpp>
#include <Sessions.hpp>
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::RapidShell::Storage;
using namespace de::rapid::shell;
using namespace Rapid::TestHelper;
using namespace Rapid::System;
using namespace Rapid::Common;
using namespace Rapid::System;

class SessionDatabaseIpcServerTestEventListener : public Catch::EventListenerBase
{
public:
    using EventListenerBase::EventListenerBase;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        auto dir = QDir{QDir::tempPath().append(QDir::separator()).append("rapid_shell")};
        if (dir.exists()) {
            REQUIRE(std::filesystem::remove_all(dir.path().toStdString()));
        }
    }

private:
    std::string mCleanDbFileName;
};

namespace
{

QString storeSession(SessionData const& session)
{
    auto dir = QDir{QDir::tempPath().append(QDir::separator()).append("rapid_shell")};
    if (not dir.exists()) {
        REQUIRE(dir.mkpath(dir.path()));
    }
    auto const filePath = dir.path()
                              .append(QDir::separator())
                              .append("%1_%2.session")
                              .arg(QString::fromStdString(session.getSessionDate().asString()),
                                   QString::fromStdString(session.getSessionTime().asString()));
    auto file = QFile{filePath};
    REQUIRE(file.open(QFile::WriteOnly));
    auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(session);
    file.write(rawJson.c_str(), static_cast<qint64>(rawJson.size()));
    return filePath;
}

} // namespace

// CATCH_REGISTER_LISTENER(SessionDatabaseIpcServerTestEventListener)

TEST_CASE("the SessionDatabaseIpcServer shall provide the session count")
{
    auto db = SessionDatabaseMock{};
    auto server = SessionDatabaseIpcServer{db};
    auto client = SessionDatabase{"de.rapid.shell", "/de/rapid/shell", QDBusConnection::sessionBus()};

    REQUIRE_CALL(db, getSessionCount()).RETURN(2);
    auto request = client.GetSessionCount();
    CHECK(QTest::qWaitFor([&request] {
        return request.isFinished();
    }));
    CHECK_FALSE(request.isError());
    REQUIRE(request.value() == 2);
}

TEST_CASE("the SessionDatabaseIpcServer shall provide read access to the session")
{
    auto db = SessionDatabaseMock{};
    auto server = SessionDatabaseIpcServer{db};
    auto client = SessionDatabase{"de.rapid.shell", "/de/rapid/shell", QDBusConnection::sessionBus()};

    SECTION("give the json path for the IPC client to read the session from the requested index")
    {
        auto const asyncResult = std::make_shared<Rapid::Storage::GetSessionResult>();
        asyncResult->setResultValue(Sessions::getTestSession());
        asyncResult->setResult(Result::Ok);
        REQUIRE_CALL(db, getSessionByIndexAsync(2)).RETURN(asyncResult);
        auto request = client.GetSessionByIndex(2);
        CHECK(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        CHECK_FALSE(request.isError());
        auto result = request.value();
        CHECK_FALSE(result.isEmpty());
        REQUIRE(result == QStringLiteral("/tmp/rapid_shell/01.01.1970_13:00:00.000.session"));
    }

    SECTION("create the file with the send to the client and the session in json format")
    {
        auto const asyncResult = std::make_shared<Rapid::Storage::GetSessionResult>();
        asyncResult->setResultValue(Sessions::getTestSession());
        asyncResult->setResult(Result::Ok);
        REQUIRE_CALL(db, getSessionByIndexAsync(2)).RETURN(asyncResult);
        auto request = client.GetSessionByIndex(2);
        CHECK(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        auto filePath = request.value();
        CHECK(QFile::exists(filePath));
        auto file = QFile(filePath);
        CHECK(file.open(QFile::ReadOnly));
        auto content = file.readAll().toStdString();
        REQUIRE(Rapid::Common::JsonDeserializer::Session::deserialize(content));
    }

    SECTION("send an error message to the caller when index is not found")
    {
        auto const asyncResult = std::make_shared<Rapid::Storage::GetSessionResult>();
        asyncResult->setResult(Result::Error);
        REQUIRE_CALL(db, getSessionByIndexAsync(2)).RETURN(asyncResult);
        auto request = client.GetSessionByIndex(2);
        CHECK(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        CHECK(request.isError());
        CHECK(request.error().type() == QDBusError::InvalidArgs);
        REQUIRE_FALSE(request.error().errorString(QDBusError::InvalidArgs).isEmpty());
    }
}

TEST_CASE("The SessionDatabaseIpcServerPrivate shall delete sessions")
{
    auto db = SessionDatabaseMock{};
    auto server = SessionDatabaseIpcServer{db};
    auto client = SessionDatabase{"de.rapid.shell", "/de/rapid/shell", QDBusConnection::sessionBus()};

    REQUIRE_CALL(db, deleteSession(2)).LR_SIDE_EFFECT(db.sessionDeleted.emit(_1));
    auto sessionDeletedSpy = QSignalSpy{&client, &SessionDatabase::SessionDeleted};
    auto request = client.DeleteSessionByIndex(2);
    REQUIRE(sessionDeletedSpy.wait());
    CHECK_FALSE(request.isError());
    REQUIRE(sessionDeletedSpy.at(0).first().value<quint32>() == 2);
}

TEST_CASE("the SessionDatabaseIpcServer shall store sessions")
{
    auto db = SessionDatabaseMock{};
    auto server = SessionDatabaseIpcServer{db};
    auto client = SessionDatabase{"de.rapid.shell", "/de/rapid/shell", QDBusConnection::sessionBus()};

    auto const filePath = storeSession(Sessions::getTestSession());
    auto const asyncResult = std::make_shared<AsyncResult>();
    asyncResult->setResult(Result::Ok);
    REQUIRE_CALL(db, storeSession(Sessions::getTestSession()))
        .LR_SIDE_EFFECT(db.sessionAdded.emit(2))
        .RETURN(asyncResult);
    auto sessionAddedSpy = QSignalSpy{&client, &SessionDatabase::SessionAdded};
    auto request = client.StoreSession(filePath);
    REQUIRE(sessionAddedSpy.wait());
    REQUIRE(request.isFinished());
    REQUIRE(request.value());
    REQUIRE(sessionAddedSpy.at(0).first().value<quint32>() == 2);
}

TEST_CASE("the SessionDatabaseIpcServer shall emit update session when the same session is stored")
{
    auto db = SessionDatabaseMock{};
    auto server = SessionDatabaseIpcServer{db};
    auto client = SessionDatabase{"de.rapid.shell", "/de/rapid/shell", QDBusConnection::sessionBus()};

    auto const filePath = storeSession(Sessions::getTestSession());
    auto const asyncResult = std::make_shared<AsyncResult>();
    asyncResult->setResult(Result::Ok);
    REQUIRE_CALL(db, storeSession(Sessions::getTestSession()))
        .LR_SIDE_EFFECT(db.sessionUpdated.emit(3))
        .RETURN(asyncResult);
    auto sessionUpdated = QSignalSpy{&client, &SessionDatabase::SessionUpdated};
    auto request = client.StoreSession(filePath);
    CHECK(sessionUpdated.wait());
    CHECK(request.isFinished());
    CHECK(request.value());
    REQUIRE(sessionUpdated.at(0).first().value<quint32>() == 3);
}

QT_CATCH2_TEST_MAIN()
