// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Connection.hpp"
#include <cassert>
#include <concepts>
#include <cstdint>
#include <optional>
#include <sqlite3.h>
#include <type_traits>

namespace Rapid::Storage::Private
{

enum class ExecuteResult
{
    Ok,
    Busy,
    Row,
    Error
};

enum class HasColumnValueResult
{
    Ok,
    Null
};

enum class ColumnType
{
    Integer = 1,
    Float = 2,
    Text = 3,
    Blob = 4,
    Null = 5
};

/**
 * A SQLite prepare statement
 */
class Statement final
{
public:
    /**
     * Constructs the statement object.
     * @param connection The db connection that shall be used to communicate with database. The connection must live as
     * long as the statment class.
     */
    Statement(Connection const& dbConnection);

    /**
     * Default destructor finialize the statement if necessary.
     */
    ~Statement();

    /**
     * Deleted copy constructor
     */
    Statement(Statement const& other) = delete;

    /**
     * Deleted copy assignment operator
     */
    Statement& operator=(Statement const& other) = delete;

    /**
     * Deleted move constructor
     */
    Statement(Statement&& ohter) = delete;

    /**
     * Deleted move assignemnt operator
     */
    Statement& operator=(Statement&& other) = delete;

    /**
     * Prepares the statment for furhter use.
     * Can be used to chain the binding.
     * If the operation was succesful can be checked with @ref Statement::hasError.
     * @param connection The database connection.
     * @param statement The state that shall be prepared.
     * @return A reference to the statement for function chainging.
     */
    Statement& prepare(char const* statement) noexcept;

    /**
     * Excutes the statement. Import is that the statement is prepared before calling execute.
     * @return "Ok" on success, the statement is successful executed and there is not more data to fetch.
     * @return "Busy" if it fails to require the lock
     * @return "Row" the statment is executed and there is still data to fetch.
     * @return "Error" in any other error case.
     */
    ExecuteResult execute() noexcept;

    /**
     * Resets the statement so it can be used with a different statement. Every fetched data is cleared.
     */
    void reset();

    /**
     * Binds a value to the given index.
     * @param index The index of the value in the prepared statement.
     * @tparam value The value for the placeholder in the statement.
     * @return "Ok" Value is succesful binded.
     * @return "InvalidIndex" The index of the value is not valid.
     * @return "Error" for any other error. Check the error message of the connection.
     */
    template <typename T>
    constexpr Statement& bindValue(std::size_t index, T const& value) noexcept
    {
        assert(mStatement != nullptr);

        if (mBindError || not mPrepared) {
            return *this;
        }

        auto result = int{0};
        if constexpr (std::is_same_v<T, int> or std::is_same_v<T, std::size_t>) {
            result = sqlite3_bind_int(mStatement, static_cast<int>(index), value);
        } else if constexpr (std::is_same_v<T, double> or std::is_same_v<T, float>) {
            result = sqlite3_bind_double(mStatement, static_cast<int>(index), value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = sqlite3_bind_text(mStatement,
                                       static_cast<int>(index),
                                       value.c_str(),
                                       static_cast<int>(value.size()),
                                       SQLITE_STATIC);
        } else {
            static_assert("Unsupported Type passed to bindValue");
        }
        if (result != SQLITE_OK) {
            mBindError = false;
        }
        return *this;
    }

    /**
     * Check for bind error.
     * It's intended use is for the chainging of binding preparing.
     * @return True a bind error happens otherwise false.
     */
    bool hasError();

    /**
     * Gives the number of columns of the result. The value is zero when the statement doesn't select anything from the
     * database, e.g. UPDATE. The column count is zero when the statement isn't prepared and not succesful executed.
     * @return The number of columns in the result.
     */
    std::size_t getColumnCount() const noexcept;

    /**
     * Gives the type for the specific column type. Is the index out of range the ColumnTpye::Null is returned.
     * @return The column type.
     */
    ColumnType getColumnType(std::size_t index) const noexcept;

    /**
     * Checks if the column has a value.
     * @return "Ok" The column has a value
     * @return "Null" The column has no value
     */
    HasColumnValueResult hasColumnValue(std::size_t index) const noexcept;

    /**
     * Gives the value for the passed index.
     * @param index The index of the column in the result row.
     * @return std::nullopt when the index is not in the range.
     */
    template <std::default_initializable T>
    constexpr std::optional<T> getColumn(std::size_t index)
    {
        if (mStatement == nullptr || getColumnCount() < index) {
            return std::nullopt;
        }

        auto result = T{};
        if constexpr (std::is_same_v<T, int>) {
            result = sqlite3_column_int(mStatement, static_cast<std::int32_t>(index));
        } else if constexpr (std::is_same_v<T, double> or std::is_same_v<T, float>) {
            result = static_cast<float>(sqlite3_column_double(mStatement, static_cast<std::int32_t>(index)));
        } else if constexpr (std::is_same_v<T, std::string>) {
            // TODO: This maybe contains a UTF-8 encoding, we need better encoding :-D
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            auto string =
                reinterpret_cast<char const*>(sqlite3_column_text(mStatement, static_cast<std::int32_t>(index)));
            result = std::string{string};
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }
        return result;
    }

private:
    sqlite3_stmt* mStatement{nullptr};
    Connection const& mDbConnection;
    bool mPrepared = false;
    bool mBindError = false;
};

} // namespace Rapid::Storage::Private

#endif // STATEMENT_HPP
