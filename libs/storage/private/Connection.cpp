// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Connection.hpp"
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace Rapid::Storage::Private
{

Connection& Connection::connection(std::string const& database)
{
    static auto connections = std::unordered_map<std::string, std::unique_ptr<Connection>>{};
    if (not connections.contains(database)) {
        connections.insert({database, std::make_unique<Connection>(database)});
    }
    return *connections[database].get();
}

Connection::~Connection()
{
    if (mHandle != nullptr) {
        sqlite3_close(mHandle);
    }
}

Connection::Connection(std::string const& database)
{
    if (mHandle != nullptr) {
        sqlite3_close(mHandle);
    }

    if (sqlite3_open_v2(database.c_str(),
                        &mHandle,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_PRIVATECACHE,
                        nullptr) == SQLITE_OK) {
        sqlite3_exec(mHandle, "PRAGMA foreign_keys = 1", nullptr, nullptr, nullptr);
        return;
    }

    spdlog::error("Exiting failed to create database connection. Error: {}", getErrorMessage());
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

} // namespace Rapid::Storage::Private
