// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LAPTIMERCORE_TEST_DUMMY_SQLITEDATABASETESTEVENTLISTENER_HPP
#define LAPTIMERCORE_TEST_DUMMY_SQLITEDATABASETESTEVENTLISTENER_HPP
#include <catch2/catch_all.hpp>
#include <unistd.h>

namespace Rapid::TestHelper::SqliteDatabaseTestHelper
{
std::string getWorkingDir();

std::string getTestDatabseFolder();

std::string getDbNameForTest() noexcept;

std::string getTestDatabaseFile(std::string const& dbFile = getDbNameForTest());

} // namespace Rapid::TestHelper::SqliteDatabaseTestHelper

namespace Rapid::TestHelper
{

class SqliteDatabaseTestEventlistener : public Catch::EventListenerBase
{
public:
    using EventListenerBase::EventListenerBase;

    [[nodiscard]] virtual std::string getCleanDbFileName() const noexcept = 0;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override;

private:
    std::string mCleanDbFileName;
};

} // namespace Rapid::TestHelper

#endif // LAPTIMERCORE_TEST_DUMMY_SQLITEDATABASETESTEVENTLISTENER_HPP
