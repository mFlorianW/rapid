// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LAPTIMERCORE_TEST_DUMMY_SQLITEDATABASETESTEVENTLISTENER_HPP
#define LAPTIMERCORE_TEST_DUMMY_SQLITEDATABASETESTEVENTLISTENER_HPP
#define CATCH_CONFIG_EXTERNAL_INTERFACES
#include <catch2/catch.hpp>
#include <filesystem>
#include <unistd.h>

namespace Rapid::TestHelper::SqliteDatabaseTestHelper
{
std::string getWorkingDir();

std::string getTestDatabseFolder();

std::string getTestDatabseFile(std::string const& dbFile = "trackmanagement_test.db");

} // namespace Rapid::TestHelper::SqliteDatabaseTestHelper

#endif // LAPTIMERCORE_TEST_DUMMY_SQLITEDATABASETESTEVENTLISTENER_HPP
