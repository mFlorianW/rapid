// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Statement.hpp"
#include "cstring"

namespace Rapid::Storage::Private
{

Statement::Statement(Connection const& dbConnection)
    : mDbConnection{dbConnection}
{
}

Statement::~Statement()
{
    if (mStatement != nullptr) {
        sqlite3_finalize(mStatement);
    }
}

PrepareResult Statement::prepare(char const* statement) noexcept
{
    mPrepared = false;
    auto* dbHandle = mDbConnection.getRawHandle();
    if (dbHandle == nullptr || statement == nullptr) {
        return PrepareResult::Error;
    }

    // If the statement is reused and shall be prepared again, the statement must be reset to it's initial state.
    if (mStatement != nullptr) {
        reset();
    }

    auto const prepareResult =
        sqlite3_prepare_v2(dbHandle, statement, static_cast<int>(std::strlen(statement)), &mStatement, nullptr);
    if (prepareResult == SQLITE_OK) {
        mPrepared = true;
        return PrepareResult::Ok;
    }

    return PrepareResult::Error;
}

Statement& Statement::prepare2(char const* statement) noexcept
{
    std::ignore = prepare(statement);
    return *this;
}

ExecuteResult Statement::execute() noexcept
{
    auto* dbHandle = mDbConnection.getRawHandle();
    if (dbHandle == nullptr) {
        return ExecuteResult::Error;
    }

    auto const stepResult = sqlite3_step(mStatement);
    if (stepResult == SQLITE_BUSY) {
        return ExecuteResult::Busy;
    } else if (stepResult == SQLITE_ROW) {
        return ExecuteResult::Row;
    } else if (stepResult != SQLITE_DONE && stepResult != SQLITE_ROW) {
        return ExecuteResult::Error;
    } else {
        return ExecuteResult::Ok;
    }
}

void Statement::reset()
{
    if (mStatement != nullptr) {
        sqlite3_reset(mStatement);
    }
}

bool Statement::hasError()
{
    return mBindError;
}

BindResult Statement::bindIntValue(std::size_t index, int32_t value) noexcept
{
    if (mStatement == nullptr) {
        return BindResult::Error;
    }

    auto const result = sqlite3_bind_int(mStatement, static_cast<int>(index), value);
    if (result == SQLITE_OK) {
        return BindResult::Ok;
    }

    return BindResult::Error;
}

BindResult Statement::bindStringValue(std::size_t index, std::string const& value)
{
    if (mStatement == nullptr) {
        return BindResult::Error;
    }

    auto const result = sqlite3_bind_text(mStatement,
                                          static_cast<int>(index),
                                          value.c_str(),
                                          static_cast<int>(value.size()),
                                          SQLITE_STATIC);
    if (result == SQLITE_OK) {
        return BindResult::Ok;
    }

    return BindResult::Error;
}

std::size_t Statement::getColumnCount() const noexcept
{
    if (mStatement == nullptr) {
        return 0;
    }

    return sqlite3_column_count(mStatement);
}

ColumnType Statement::getColumnType(std::size_t index) const noexcept
{
    if (mStatement == nullptr || index > getColumnCount()) {
        return ColumnType::Null;
    }

    auto const type = sqlite3_column_type(mStatement, static_cast<int>(index));
    switch (type) {
    case 1:
        return ColumnType::Integer;
    case 2:
        return ColumnType::Float;
    case 3:
        return ColumnType::Text;
    case 4:
        return ColumnType::Blob;
    default:
        return ColumnType::Null;
    }
}

HasColumnValueResult Statement::hasColumnValue(std::size_t index) const noexcept
{
    if (index > getColumnCount() || mStatement == nullptr) {
        return HasColumnValueResult::Null;
    }

    return sqlite3_column_type(mStatement, static_cast<int>(index)) != SQLITE_NULL ? HasColumnValueResult::Ok
                                                                                   : HasColumnValueResult::Null;
}

std::optional<int32_t> Statement::getIntColumn(std::size_t index) const noexcept
{
    if (mStatement == nullptr || getColumnCount() < index) {
        return std::nullopt;
    }

    return sqlite3_column_int(mStatement, static_cast<std::int32_t>(index));
}

std::optional<float> Statement::getFloatColumn(std::size_t index) const noexcept
{
    if (mStatement == nullptr || getColumnCount() < index) {
        return std::nullopt;
    }

    return static_cast<float>(sqlite3_column_double(mStatement, static_cast<std::int32_t>(index)));
}

std::optional<std::string> Statement::getStringColumn(std::size_t index) const noexcept
{
    if (mStatement == nullptr || getColumnCount() < index) {
        return std::nullopt;
    }

    // TODO: This maybe contains a UTF-8 encoding, we need better encoding :-D
    // NOLINTBEGIN
    auto string = reinterpret_cast<char const*>(sqlite3_column_text(mStatement, static_cast<std::int32_t>(index)));
    return std::string{string};
    // NOLINTEND
}

} // namespace Rapid::Storage::Private
