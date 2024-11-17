// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <sqlite3.h>
#include <string>

namespace Rapid::Storage::Private
{

/**
 * This Sqlite database connection. A connection is unique and is not copyable nor movable.
 */
class Connection final
{
public:
    /**
     * Create the connection instance for the process it's only possible to have connection
     * per process. This for the reason to correctly handle changes on the database.
     * @param database The path to the SQLite Database file.
     */
    static Connection& connection(std::string const& database);

    /**
     * Tries to open the sqlite3 database for the given string.
     * @param database The path to the database.
     */
    Connection(std::string const& database);

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

private:
    sqlite3* mHandle{nullptr};
};

} // namespace Rapid::Storage::Private

#endif // CONNECTION_HPP
