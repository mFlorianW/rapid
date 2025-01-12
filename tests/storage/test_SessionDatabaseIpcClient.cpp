// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QSignalSpy>
#include <QTest>
#include <SessionDatabaseAdaptor.h>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>
#include <common/JsonDeserializer.hpp>
#include <common/JsonSerializer.hpp>
#include <common/SessionData.hpp>
#include <spdlog/spdlog.h>
#include <storage/qt/SessionDatabaseIpcClient.hpp>
#include <system/qt/RapidApplication.hpp>
#include <testhelper/Sessions.hpp>
#include <testhelper/qt/QtTestHelper.hpp>

using namespace Rapid::Storage::Qt;
using namespace Rapid::Storage;
using namespace Rapid::Common;
using namespace Rapid::System;
using namespace Rapid::TestHelper;

namespace
{

struct TestFixture
{
    SessionDatabaseServer server;
    SessionDatabaseAdaptor adaptor{&server};
    QDir dir = QDir{QDir::tempPath().append(QDir::separator()).append("rapid_shell")};

    TestFixture()
    {
        if (dir.exists()) {
            REQUIRE(std::filesystem::remove_all(dir.path().toStdString()));
        }
    }

    void waitForInit(SessionDatabaseIpcClient& ipcClient)
    {
        QSignalSpy initSpy = QSignalSpy{&ipcClient, &SessionDatabaseIpcClient::initialized};
        REQUIRE(QTest::qWaitFor([&ipcClient, &initSpy] {
            return ipcClient.isInitialized() and initSpy.size() == 1;
        }));
    }

    QString createSessionRequest(SessionData const& session)
    {
        REQUIRE(dir.mkpath(dir.path()));
        auto const filePath = dir.path()
                                  .append(QDir::separator())
                                  .append("%1_%2.session")
                                  .arg(QString::fromStdString(session.getSessionDate().asString()),
                                       QString::fromStdString(session.getSessionTime().asString()));
        auto file = QFile{filePath};
        auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(session);
        createFile(filePath, rawJson);
        return filePath;
    }

    QString createSessionMetaDataRequest(SessionMetaData const& session)
    {
        REQUIRE(dir.mkpath(dir.path()));
        auto const filePath = dir.path()
                                  .append(QDir::separator())
                                  .append("%1_%2.session")
                                  .arg(QString::fromStdString(session.getSessionDate().asString()),
                                       QString::fromStdString(session.getSessionTime().asString()));
        auto rawJson = Rapid::Common::JsonSerializer::Session::serialize(session);
        createFile(filePath, rawJson);
        return filePath;
    }

    void createFile(QString const& filePath, std::string const& rawJson)
    {
        auto file = QFile{filePath};
        REQUIRE(file.open(QFile::WriteOnly));
        file.write(rawJson.c_str(), static_cast<qint64>(rawJson.size()));
    }
};

} // namespace

TEST_CASE_METHOD(TestFixture, "The SessionDatabaseIpcClient shall provide the stored session count")
{
    constexpr auto expectedSessionCount = std::size_t{2};
    REQUIRE_CALL(server, GetSessionCount()).RETURN(expectedSessionCount);
    SessionDatabaseIpcClient ipcClient = SessionDatabaseIpcClient{};
    QSignalSpy initSpy = QSignalSpy{&ipcClient, &SessionDatabaseIpcClient::initialized};
    waitForInit(ipcClient);
    REQUIRE(ipcClient.getSessionCount() == expectedSessionCount);
}

TEST_CASE_METHOD(TestFixture, "The SessionDatabaseIpcClient shall delete a session and emit the delete signal")
{
    constexpr auto expectedSessionCount = std::size_t{2};
    ALLOW_CALL(server, GetSessionCount()).RETURN(0);
    REQUIRE_CALL(server, DeleteSessionByIndex(trompeloeil::_))
        .WITH(_1 == expectedSessionCount)
        .LR_SIDE_EFFECT(Q_EMIT adaptor.SessionDeleted(_1));
    SessionDatabaseIpcClient ipcClient = SessionDatabaseIpcClient{};
    waitForInit(ipcClient);
    bool deleted = false;
    std::ignore = ipcClient.sessionDeleted.connect([&deleted](std::size_t index) {
        if (index == expectedSessionCount) {
            deleted = true;
        }
    });
    ipcClient.deleteSession(expectedSessionCount);
    REQUIRE(QTest::qWaitFor([&deleted] {
        return deleted == true;
    }));
}

TEST_CASE_METHOD(TestFixture, "The SessionDatabaseIpcClient shall deserialize the requested session and return it")
{
    constexpr auto expectedSessionCount = std::size_t{2};
    ALLOW_CALL(server, GetSessionCount()).RETURN(0);
    REQUIRE_CALL(server, GetSessionByIndex(trompeloeil::_))
        .WITH(_1 == expectedSessionCount)
        .LR_RETURN(createSessionRequest(Sessions::getTestSession()));
    SessionDatabaseIpcClient ipcClient = SessionDatabaseIpcClient{};
    waitForInit(ipcClient);
    auto result = ipcClient.getSessionByIndexAsync(expectedSessionCount);
    REQUIRE(QTest::qWaitFor([&result] {
        return result->getResult() == Result::Ok;
    }));
    REQUIRE(result->getResultValue() == Sessions::getTestSession());
}

TEST_CASE_METHOD(TestFixture,
                 "The SessionDatabaseIpcClient shall store a session and shall emit the session store signals")
{
    auto const expectedPath = QDir{QDir::tempPath().append(QDir::separator()).append("rapid_shell")};
    auto const expectedFile = expectedPath.path().append(QDir::separator()).append("01.01.1970_13:00:00.000.session");
    constexpr auto expectedSessionCount = std::size_t{2};
    ALLOW_CALL(server, GetSessionCount()).RETURN(0);
    SessionDatabaseIpcClient ipcClient = SessionDatabaseIpcClient{};
    waitForInit(ipcClient);

    SECTION("The Correct path is transmitted to the server")
    {
        REQUIRE_CALL(server, StoreSession(trompeloeil::_)).LR_WITH(_1 == expectedFile).RETURN(true);
        auto result = ipcClient.storeSession(Sessions::getTestSession());
    }

    SECTION("The created file for the server is correct created")
    {
        REQUIRE_CALL(server, StoreSession(trompeloeil::_)).RETURN(true);
        auto result = ipcClient.storeSession(Sessions::getTestSession());
        REQUIRE(QTest::qWaitFor([&result] {
            return result->getResult() == Result::Ok;
        }));
        auto sessionFile = QFile{expectedFile};
        REQUIRE(sessionFile.exists());
        REQUIRE(sessionFile.open(QFile::ReadOnly));
        auto content = sessionFile.readAll().toStdString();
        REQUIRE(JsonDeserializer::Session::deserialize(content).value_or(SessionData{}) == Sessions::getTestSession());
    }

    SECTION("The session added signal is correctly emitted when the server emits a session added status")
    {
        REQUIRE_CALL(server, StoreSession(trompeloeil::_))
            .LR_SIDE_EFFECT(Q_EMIT adaptor.SessionAdded(expectedSessionCount))
            .RETURN(true);
        auto result = ipcClient.storeSession(Sessions::getTestSession());
        bool added = false;
        std::ignore = ipcClient.sessionAdded.connect([&added](std::size_t index) {
            if (index == expectedSessionCount) {
                added = true;
            }
        });
        REQUIRE(QTest::qWaitFor([&added] {
            return added;
        }));
        REQUIRE(result->getResult() == Result::Ok);
    }

    SECTION("The session updated signal is correctly emitted when the server emits a session added status")
    {
        REQUIRE_CALL(server, StoreSession(trompeloeil::_))
            .LR_SIDE_EFFECT(Q_EMIT adaptor.SessionUpdated(expectedSessionCount))
            .RETURN(true);
        auto result = ipcClient.storeSession(Sessions::getTestSession());
        bool updated = false;
        std::ignore = ipcClient.sessionUpdated.connect([&updated](std::size_t index) {
            if (index == expectedSessionCount) {
                updated = true;
            }
        });
        REQUIRE(QTest::qWaitFor([&updated] {
            return updated;
        }));
        REQUIRE(result->getResult() == Result::Ok);
    }

    SECTION("Store the same session twice so the temp file have to rewritten")
    {
        REQUIRE_CALL(server, StoreSession(trompeloeil::_)).TIMES(2).RETURN(true);
        auto result = ipcClient.storeSession(Sessions::getTestSession());
        REQUIRE(QTest::qWaitFor([&result] {
            return result->getResult() == Result::Ok;
        }));
        result = ipcClient.storeSession(Sessions::getTestSession());
        REQUIRE(QTest::qWaitFor([&result] {
            return result->getResult() == Result::Ok;
        }));
    }
}

TEST_CASE_METHOD(TestFixture, "The SessionDatabaseIpcClient shall give the requested session meta data")
{
    constexpr auto expectedSessionCount = std::size_t{1};
    ALLOW_CALL(server, GetSessionCount()).RETURN(2);
    REQUIRE_CALL(server, GetSessionMetaDataByIndex(trompeloeil::_))
        .WITH(_1 == expectedSessionCount)
        .LR_RETURN(createSessionMetaDataRequest(Sessions::getTestSessionMetaData()));
    SessionDatabaseIpcClient ipcClient = SessionDatabaseIpcClient{};
    waitForInit(ipcClient);
    auto result = ipcClient.getSessionMetaDataByIndexAsync(expectedSessionCount);
    REQUIRE(QTest::qWaitFor([&result] {
        return result->getResult() == Result::Ok;
    }));
    auto const maybeResult = result->getResultValue();
    auto const sessionMetdaData = maybeResult.value_or(SessionMetaData{});
    auto const expectedMetaData = Sessions::getTestSessionMetaData();
    CHECK(sessionMetdaData.getSessionDate() == expectedMetaData.getSessionDate());
    CHECK(sessionMetdaData.getSessionTime() == expectedMetaData.getSessionTime());
    REQUIRE(sessionMetdaData.getTrack() == expectedMetaData.getTrack());
}

TEST_CASE_METHOD(TestFixture, "The SessionDatabaseIpcClient shall give the requested session for the meta data")
{
    constexpr auto expectedSessionCount = std::size_t{1};
    auto const session = Sessions::getTestSession3();
    ALLOW_CALL(server, GetSessionCount()).RETURN(expectedSessionCount);
    REQUIRE_CALL(server, GetSessionByMetaData(trompeloeil::_)).LR_RETURN(createSessionRequest(session));
    auto ipcClient = SessionDatabaseIpcClient{};
    auto const metaData = Sessions::getTestSessionMetaData();
    auto const expectedSessionMetaDataName =
        dir.absolutePath().append(QDir::separator()).append("01.01.1970_13:00:00.000.sessionMetaData");
    waitForInit(ipcClient);

    SECTION("The correct path is transmitted to the server")
    {
        ipcClient.getSessionByMetadataAsync(metaData);
        REQUIRE(QFile::exists(expectedSessionMetaDataName));
    }

    SECTION("The session meta data are correct serialized and stored to the file")
    {
        auto result = ipcClient.getSessionByMetadataAsync(metaData);
        REQUIRE(QTest::qWaitFor([&result] {
            return result->getResult() == Rapid::System::Result::Ok;
        }));
        auto file = QFile{expectedSessionMetaDataName};
        REQUIRE(file.open(QFile::ReadOnly));
        auto const readMetaData = JsonDeserializer::SessionMetaData::deserialize(file.readAll().toStdString());
        REQUIRE(metaData == readMetaData.value_or(SessionMetaData{}));
    }

    SECTION("The SessionData returned from the Database is correct deserialized")
    {
        auto const result = ipcClient.getSessionByMetadataAsync(metaData);
        REQUIRE(QTest::qWaitFor([&result] {
            return result->getResult() == Result::Ok;
        }));
        REQUIRE(result->getResultValue() == session);
    }
}

QT_CATCH2_TEST_MAIN()
