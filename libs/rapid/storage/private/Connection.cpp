// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Connection.hpp"
#include <spdlog/spdlog.h>

#include <utility>

namespace Rapid::Storage::Private
{

std::unordered_map<std::string, std::weak_ptr<Connection>> Connection::sConnections =
    std::unordered_map<std::string, std::weak_ptr<Connection>>{};

std::shared_ptr<Connection> Connection::connection(std::string const& database)
{
    if (not sConnections.contains(database)) {
        SPDLOG_INFO("Create database connection for db: {}", database);
        auto connection = std::make_shared<Connection>(database);
        sConnections.insert({database, connection});
        return connection;
    }
    SPDLOG_INFO("Reuse database connection for db: {}", database);
    return sConnections[database].lock();
}

Connection::~Connection()
{
    if (mHandle != nullptr) {
        sqlite3_close(mHandle);
    }
    if (sConnections.contains(mDatabase)) {
        auto wConnection = sConnections.at(mDatabase);
        SPDLOG_INFO("Cleanup database connection for database. {}", mDatabase);
        if (wConnection.use_count() == 0) {
            sConnections.erase(mDatabase);
        }
    }
}

Connection::Connection(std::string database)
    : mDatabase{std::move(database)}
{
    if (mHandle != nullptr) {
        sqlite3_close(mHandle);
    }

    if (sqlite3_open_v2(mDatabase.c_str(),
                        &mHandle,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_PRIVATECACHE,
                        nullptr) == SQLITE_OK) {
        sqlite3_exec(mHandle, "PRAGMA foreign_keys = 1", nullptr, nullptr, nullptr);
        sqlite3_exec(mHandle, "PRAGMA journal_mode = wal", nullptr, nullptr, nullptr);
        return;
    }

    SPDLOG_ERROR("Exiting failed to create database connection. Error: {}", getErrorMessage());
    std::exit(255);
}

std::string Connection::getErrorMessage() const noexcept
{
    if (mHandle != nullptr) {
        return sqlite3_errmsg(mHandle);
    }

    return {};
}

sqlite3* Connection::getRawHandle() const noexcept
{
    if (mHandle == nullptr) {
        return nullptr;
    }

    return mHandle;
}

void Connection::beginTransaction()
{
    sqlite3_exec(mHandle, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
}

void Connection::commitTransaction()
{
    sqlite3_exec(mHandle, "COMMIT", nullptr, nullptr, nullptr);
}

void Connection::rollback()
{
    sqlite3_exec(mHandle, "ROLLBACK", nullptr, nullptr, nullptr);
}

CommitGuard::CommitGuard(Connection& connection)
    : mConnection{connection}
{
    mConnection.beginTransaction();
}

CommitGuard::~CommitGuard()
{
    if (mRollback) {

    } else {
        mConnection.commitTransaction();
    }
}

void CommitGuard::setRollback()
{
    mRollback = true;
}

} // namespace Rapid::Storage::Private
