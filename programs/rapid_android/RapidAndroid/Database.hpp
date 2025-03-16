// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_ANDROID_DATABASE_HPP
#define RAPID_ANDROID_DATABASE_HPP

#include <QString>

namespace Rapid::Android
{

/**
 * @brief Setups the database.
 *
 * @details The setup only setups the database when no database existis.
 *
 * @return The path to the database for further use.
 */
std::optional<QString> setupDatabase();

/**
 * @brief Gives the path of the database
 *
 * @details The function only gives absolute path, there is no validation if the database exists.
 *          So before trying to connect to the database make sure to call @ref Rapid::Android::Database::setupDatabase
 *
 * @return The absolute path to the database.
 */
QString getDatabaseLocation() noexcept;

} // namespace Rapid::Android

#endif // !RAPID_ANDROID_DATABASE_HPP
