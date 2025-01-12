// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "TestSessionDatabaseIpcClient.h"
#include <QCoreApplication>
#include <QDeadlineTimer>
#include <QFile>
#include <QSignalSpy>
#include <QTest>
#include <SessionDatabaseIpcServer.hpp>
#include <catch2/catch_all.hpp>
#include <common/JsonDeserializer.hpp>
#include <common/JsonSerializer.hpp>
#include <spdlog/spdlog.h>
#include <system/qt/RapidApplication.hpp>
#include <testhelper/SessionDatabaseMock.hpp>
#include <testhelper/Sessions.hpp>
#include <testhelper/qt/QtTestHelper.hpp>

using namespace Rapid::RapidShell::Storage;
using namespace de::rapid::shell;
using namespace Rapid::TestHelper;
using namespace Rapid::System;
using namespace Rapid::Common;
using namespace Rapid::System;

namespace
{

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

struct TestFixture
{
    SessionDatabaseMock db;
    SessionDatabaseIpcServer server = SessionDatabaseIpcServer{db};
    SessionDatabase client = SessionDatabase{"de.rapid.shell", "/de/rapid/shell", QDBusConnection::sessionBus()};
    QDir dir{QDir::tempPath().append(QDir::separator()).append("rapid_shell")};

    TestFixture()
    {
        if (not dir.exists()) {
            REQUIRE(dir.mkpath(dir.path()));
        }
    }

    QString storeSession(SessionData const& session)
    {
        auto const filePath = dir.path()
                                  .append(QDir::separator())
                                  .append("%1_%2.session")
                                  .arg(QString::fromStdString(session.getSessionDate().asString()),
                                       QString::fromStdString(session.getSessionTime().asString()));
        auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(session);
        writeFile(filePath, rawJson);
        return filePath;
    }

    QString storeSessionMetaData(SessionMetaData const& sessionMetaData)
    {
        auto const filePath = dir.path()
                                  .append(QDir::separator())
                                  .append("%1_%2.sessionMetaData")
                                  .arg(QString::fromStdString(sessionMetaData.getSessionDate().asString()),
                                       QString::fromStdString(sessionMetaData.getSessionTime().asString()));
        auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(sessionMetaData);
        writeFile(filePath, rawJson);
        return filePath;
    }

    void writeFile(QString const& filePath, std::string const& rawJson)
    {
        auto file = QFile{filePath};
        REQUIRE(file.open(QFile::WriteOnly));
        file.write(rawJson.c_str(), static_cast<qint64>(rawJson.size()));
    }
};

} // namespace

// CATCH_REGISTER_LISTENER(SessionDatabaseIpcServerTestEventListener)

TEST_CASE_METHOD(TestFixture, "the SessionDatabaseIpcServer shall provide the session count")
{
    REQUIRE_CALL(db, getSessionCount()).RETURN(2);
    auto request = client.GetSessionCount();
    CHECK(QTest::qWaitFor([&request] {
        return request.isFinished();
    }));
    CHECK_FALSE(request.isError());
    REQUIRE(request.value() == 2);
}

TEST_CASE_METHOD(TestFixture, "the SessionDatabaseIpcServer shall provide read access to the session")
{
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

    SECTION("create the file with the session data in json format")
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

TEST_CASE_METHOD(TestFixture, "The SessionDatabaseIpcServerPrivate shall delete sessions")
{
    REQUIRE_CALL(db, deleteSession(2)).LR_SIDE_EFFECT(db.sessionDeleted.emit(_1));
    auto sessionDeletedSpy = QSignalSpy{&client, &SessionDatabase::SessionDeleted};
    auto request = client.DeleteSessionByIndex(2);
    REQUIRE(sessionDeletedSpy.wait());
    CHECK_FALSE(request.isError());
    REQUIRE(sessionDeletedSpy.at(0).first().value<quint32>() == 2);
}

TEST_CASE_METHOD(TestFixture, "the SessionDatabaseIpcServer shall store sessions")
{
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

TEST_CASE_METHOD(TestFixture, "the SessionDatabaseIpcServer shall emit update session when the same session is stored")
{
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

TEST_CASE_METHOD(TestFixture, "the SessionDatabaseIpcServer shall provide read access to the session meta data")
{
    SECTION("give the json path for the IPC client to read the session from the requested index")
    {
        auto const asyncResult = std::make_shared<Rapid::Storage::GetSessionMetaDataResult>();
        asyncResult->setResultValue(Sessions::getTestSessionMetaData());
        asyncResult->setResult(Result::Ok);
        REQUIRE_CALL(db, getSessionMetaDataByIndexAsync(2)).RETURN(asyncResult);
        auto request = client.GetSessionMetaDataByIndex(2);
        CHECK(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        CHECK_FALSE(request.isError());
        auto result = request.value();
        CHECK_FALSE(result.isEmpty());
        REQUIRE(result == QStringLiteral("/tmp/rapid_shell/01.01.1970_13:00:00.000.sessionMetaData"));
    }

    SECTION("create the file with the session meta data in json format")
    {
        auto const asyncResult = std::make_shared<Rapid::Storage::GetSessionMetaDataResult>();
        asyncResult->setResultValue(Sessions::getTestSession());
        asyncResult->setResult(Result::Ok);
        REQUIRE_CALL(db, getSessionMetaDataByIndexAsync(2)).RETURN(asyncResult);
        auto request = client.GetSessionMetaDataByIndex(2);
        CHECK(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        auto filePath = request.value();
        CHECK(QFile::exists(filePath));
        auto file = QFile(filePath);
        CHECK(file.open(QFile::ReadOnly));
        auto content = file.readAll().toStdString();
        REQUIRE(Rapid::Common::JsonDeserializer::SessionMetaData::deserialize(content));
    }

    SECTION("send an error message to the caller when index is not found")
    {
        auto const asyncResult = std::make_shared<Rapid::Storage::GetSessionMetaDataResult>();
        asyncResult->setResult(Result::Error);
        REQUIRE_CALL(db, getSessionMetaDataByIndexAsync(2)).RETURN(asyncResult);
        auto request = client.GetSessionMetaDataByIndex(2);
        CHECK(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        CHECK(request.isError());
        CHECK(request.error().type() == QDBusError::InvalidArgs);
        REQUIRE_FALSE(request.error().errorString(QDBusError::InvalidArgs).isEmpty());
    }
}

TEST_CASE_METHOD(TestFixture, "the SessionDatabaseIpcServer shall provide session data for session meta data")
{
    auto const asyncResult = std::make_shared<Rapid::Storage::GetSessionResult>();
    asyncResult->setResultValue(Sessions::getTestSession3());
    asyncResult->setResult(Result::Ok);
    auto path = QStringLiteral("/tmp/rapid_shell/01.02.1970_13:00:00.000.sessionMetaData");

    SECTION("shall return an error when sesion meta data not found")
    {
        auto request = client.GetSessionByMetaData("/tmp/asdfasdfaaa");
        REQUIRE(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        REQUIRE(request.isError());
        REQUIRE(request.error().type() == QDBusError::InvalidArgs);
    }

    SECTION("shall return the session path for the passed meta data")
    {
        auto session = Sessions::getTestSession3();
        REQUIRE_CALL(db, getSessionByMetadataAsync(trompeloeil::_)).RETURN(asyncResult);
        storeSessionMetaData(session);
        auto request = client.GetSessionByMetaData(path);
        REQUIRE(QTest::qWaitFor([&request] {
            return request.isFinished();
        }));
        CHECK_FALSE(request.isError());
        auto filePath = request.value();
        CHECK(QFile::exists(filePath));
        auto file = QFile(filePath);
        CHECK(file.open(QFile::ReadOnly));
        auto content = file.readAll().toStdString();
        auto maybeSession = Rapid::Common::JsonDeserializer::Session::deserialize(content);
        REQUIRE(maybeSession.value_or(SessionData{}) == session);
    }
}

QT_CATCH2_TEST_MAIN()
