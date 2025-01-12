// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "storage/ISessionDatabase.hpp"
#include <QDBusPendingCallWatcher>
#include <QObject>
#include <QPointer>

/**
 * Forward declaration for the DBus interface
 */
class DeRapidShellSessionDatabaseInterface;

namespace Rapid::Storage::Qt
{

/**
 * Client side communication endpoint to allow applications to have access to the session part of the database.
 * The server side of the communication lives in the @ref Rapid::RapidShell::RapidShell.
 * That means for the client to work that the @ref RapidShell::RapidShell must run.
 */
class SessionDatabaseIpcClient : public QObject, public ISessionDatabase
{
    Q_OBJECT
public:
    /**
     * Creates an instance of @ref SessionDatabaseIpcClient.
     * Every instance needs to perfrom some asynchronous initialisation on creation.
     * So before calling any operations on the instance, it must be waited for the itnialized signal to be emitted or the call @ref SessionDatabaseIpcClient::isInitialized is true.
     */
    SessionDatabaseIpcClient();

    /**
     * default destructor
     */
    ~SessionDatabaseIpcClient() override;

    /**
     * Deleted copy constructor
     */
    SessionDatabaseIpcClient(SessionDatabaseIpcClient const&) = delete;

    /**
     * Deleted copy operator
     */
    SessionDatabaseIpcClient& operator=(SessionDatabaseIpcClient const&) = delete;

    /**
     * Deleted move constructor
     */
    SessionDatabaseIpcClient(SessionDatabaseIpcClient&&) noexcept = delete;

    /**
     * Deleted move operator
     */
    SessionDatabaseIpcClient& operator=(SessionDatabaseIpcClient&&) noexcept = delete;

    /**
     * @brief Gives the initialisation status of this SessionDatabaseIpcClient.
     *
     * Checks if the initialisation of this instance has finished.
     * Before the initialisation is finished the signal @ref SessionDatabaseIpcClient::initialized is emited.
     *
     * @return True initialized otherwise false
     */
    bool isInitialized();

    /**
     * @copydoc @ref ISessionDatabase::getSessionCount
     */
    std::size_t getSessionCount() override;

    /**
     * Not implented for the @ref SessionDatabaseIpcClient
     */
    std::optional<Common::SessionData> getSessionByIndex(std::size_t index) const noexcept override;

    /**
     * @copydoc @ref ISessionDatabase::getSessionByIndexAsync
     */
    std::shared_ptr<GetSessionResult> getSessionByIndexAsync(std::size_t index) noexcept override;

    /**
     * @copydoc @ref ISessionDatabase::getSessionByIndexAsync
     */
    std::shared_ptr<GetSessionResult> getSessionByMetadataAsync(
        Common::SessionMetaData const& session) noexcept override;

    /**
     * @copydoc @ref ISessionDatabase::getSessionMetaDataByIndexAsync
     */
    std::shared_ptr<GetSessionMetaDataResult> getSessionMetaDataByIndexAsync(std::size_t index) noexcept override;

    /**
     * @copydoc @ref ISessionDatabase::storeSession
     */
    std::shared_ptr<System::AsyncResult> storeSession(Common::SessionData const& session) override;

    /**
     * @copydoc @ref ISessionDatabase::deleteSession
     */
    void deleteSession(std::size_t index) override;

Q_SIGNALS:
    void initialized();

private Q_SLOTS:
    void handleSessionResponse(QDBusPendingCallWatcher* self, std::shared_ptr<Rapid::Storage::GetSessionResult> result);
    void handleSessionMetaDataResponse(QDBusPendingCallWatcher* self,
                                       std::shared_ptr<Rapid::Storage::GetSessionMetaDataResult> result);

private:
    [[nodiscard]] std::optional<QString> writeExchangeFile(QString const& fileName,
                                                           std::string const& content) const noexcept;
    [[nodiscard]] std::optional<Common::SessionData> readExchangedSession(QString const& path) const noexcept;
    [[nodiscard]] std::optional<Common::SessionMetaData> readExchangedSessionMetaData(
        QString const& path) const noexcept;

    std::unique_ptr<DeRapidShellSessionDatabaseInterface> mInterface;
    std::unordered_map<QDBusPendingCallWatcher*, std::shared_ptr<QDBusPendingCallWatcher>> mPendingCalls;
    bool mInitialized = false;
    std::size_t mSessionCount = 0U;
};

} // namespace Rapid::Storage::Qt
