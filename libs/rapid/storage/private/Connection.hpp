// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <memory>
#include <sqlite3.h>
#include <string>
#include <unordered_map>

namespace Rapid::Storage::Private
{

/**
 * This Sqlite database connection. A connection is unique and is not copyable nor movable.
 */
class Connection final
{
public:
    /**
     * Create the connection instance for the process it's only possible to have connection per process.
     * The reason for this is to correctly handle changes on the database.
     * @param database The path to the SQLite Database file.
     * @return The connection for the database.
     */
    static std::shared_ptr<Connection> connection(std::string const& database);

    /**
     * Tries to open the sqlite3 database for the given string.
     * @param database The path to the database.
     */
    Connection(std::string database);

    /**
     * Default empty constructor
     */
    ~Connection();

    /**
     * Deleted copy constructor
     */
    Connection(Connection const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    Connection& operator=(Connection const& other) = delete;

    /**
     * Deleted move constructor
     */
    Connection(Connection&& ohter) = delete;

    /**
     * Deleted move assignemnt operator
     */
    Connection& operator=(Connection&& other) = delete;

    /**
     * Gives the last reported error message of the database. The function should be called when an operation of the
     * database failed. The error message is only after calling  @ref open() with OpenResult::Successful.
     * @return The error message of the database as string.
     */
    std::string getErrorMessage() const noexcept;

    /**
     * Gives the raw handle to the database. It's only valid after calling @open() with a positive result.
     * @return If connection is correctly created the valid handle otherwise a nullptr.
     */
    sqlite3* getRawHandle() const noexcept;

    /**
     * Begins a transaction on the database.
     */
    void beginTransaction();

    /**
     * Commits the latest transactions on the database.
     */
    void commitTransaction();

    /**
     * Reverted the changes made since beginTransaction
     */
    void rollback();

private:
    sqlite3* mHandle{nullptr};
    std::string mDatabase;
    static std::unordered_map<std::string, std::weak_ptr<Connection>> sConnections;
};

class CommitGuard
{
public:
    /**
     * Begins the transcation
     * @param The connection on which the transaction shall be started.
     */
    CommitGuard(Connection& connection);

    /**
     * Commits the transaction
     */
    ~CommitGuard();

    /**
     * Marks the @ref CommitGurad to revert the transactions instead of commiting.
     */
    void setRollback();

    /**
     * Disabled move constructor
     */
    CommitGuard(CommitGuard const&) = delete;

    /**
     * Disabled copy operator
     */
    CommitGuard& operator=(CommitGuard const&) = delete;

    /**
     * Disabled move constructor
     */
    CommitGuard(CommitGuard&&) noexcept = delete;

    /**
     * Disabled copy operator
     */
    CommitGuard& operator=(CommitGuard&&) noexcept = delete;

private:
    Connection& mConnection;
    bool mRollback = false;
};

} // namespace Rapid::Storage::Private

#endif // CONNECTION_HPP
