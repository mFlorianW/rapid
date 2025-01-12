// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Statement.hpp"
#include "cstring"
#include <spdlog/spdlog.h>

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

Statement& Statement::prepare(char const* statement) noexcept
{
    mPrepared = false;
    auto* dbHandle = mDbConnection.getRawHandle();
    if (dbHandle == nullptr or statement == nullptr) {
        mBindError = true;
    }

    // If the statement is reused and shall be prepared again, the statement must be reset to it's initial state.
    if (mStatement != nullptr) {
        reset();
    }

    auto const prepareResult =
        sqlite3_prepare_v2(dbHandle, statement, static_cast<int>(std::strlen(statement)), &mStatement, nullptr);
    if (prepareResult == SQLITE_OK) {
        mPrepared = true;
    } else {
        spdlog::error("Failed to prepare statement {} Error: {}", statement, mDbConnection.getErrorMessage());
    }

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

} // namespace Rapid::Storage::Private
