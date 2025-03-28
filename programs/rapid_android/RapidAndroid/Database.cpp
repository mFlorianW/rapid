// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Database.hpp"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <spdlog/spdlog.h>

#ifdef ANDROID
constexpr auto DATABASE_FILE = ":/rapid_android/rapid.db";
#else
#include <DatabaseFile.hpp>
#endif

namespace
{

QStandardPaths::StandardLocation writeLocation()
{
#ifdef ANDROID
    return QStandardPaths::StandardLocation::AppDataLocation;
#else
    return QStandardPaths::StandardLocation::GenericDataLocation;
#endif
}

} // namespace

namespace Rapid::Android
{

std::optional<QString> setupDatabase()
{
    auto const dbFile = getDatabaseLocation();
    auto const dbLocation = QFileInfo{dbFile}.absolutePath();
    if (not QFile::exists(dbFile)) {
        SPDLOG_INFO("No database found! Copy empty default database to {}", dbLocation.toStdString());
        auto dbDir = QDir{};
        if (not dbDir.exists(dbLocation)) {
            SPDLOG_INFO("Database location {} doesn't exists", dbLocation.toStdString());
            if (not dbDir.mkdir(dbLocation)) {
                SPDLOG_ERROR("Failed to create database location {}", dbFile.toStdString());
                return std::nullopt;
            } else {
                SPDLOG_INFO("Database location {} exists", dbLocation.toStdString());
            }
        } else {
            SPDLOG_INFO("Database location {} exists", dbLocation.toStdString());
        }

        if (not QFile::copy(DATABASE_FILE, dbFile)) {
            SPDLOG_ERROR("Failed to copy empty database to: {}", dbFile.toStdString());
            return std::nullopt;
        } else {
            SPDLOG_INFO("Copied empty database to: {}", dbFile.toStdString());
            return dbFile;
        }
    }
    SPDLOG_INFO("Database file exists in {}", dbFile.toStdString());
    return dbFile;
}

bool setupDatabaseFilePermissions()
{
    if (QFile(Rapid::Android::getDatabaseLocation()).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
        SPDLOG_INFO("Succcesful set write permissions to owner of the Database in MAIN function");
        return true;
    }
    SPDLOG_ERROR("Failed to set write permissions to owner of the Database in MAIN function");
    return false;
}

QString getDatabaseLocation() noexcept
{
    auto const dbLocation =
        QString{"%1/%2"}.arg(QStandardPaths::writableLocation(writeLocation()), QStringLiteral("rapid"));
    auto const dbFile = QString{"%1%2"}.arg(dbLocation, QStringLiteral("/rapid.db"));
    return dbFile;
}

} // namespace Rapid::Android
