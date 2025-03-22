// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <ContextMenuModel.hpp>
#include <catch2/catch_all.hpp>

namespace
{

struct TestFixture
{
    std::vector<Rapid::Android::ContextMenuEntry> entries{
        Rapid::Android::ContextMenuEntry{QStringLiteral("123"), QStringLiteral("456")},
        Rapid::Android::ContextMenuEntry{QStringLiteral("78"), QStringLiteral("9")}};
};

} // namespace

TEST_CASE_METHOD(TestFixture, "The ContextMenuModel shall return the correct role names")
{
    auto model = Rapid::Android::ContextMenuModel{};
    REQUIRE(model.roleNames() ==
            QHash<int, QByteArray>{{Rapid::Android::ContextMenuModel::DisplayRole::Entry, "entry"}});
}

TEST_CASE_METHOD(TestFixture, "The ContextMenuModel shall return the correct data")
{
    auto model = Rapid::Android::ContextMenuModel{entries};
    SECTION("Correct row count is returnded")
    {
        REQUIRE(model.rowCount({}) == 2);
    }

    SECTION("Correct data for the role is returned")
    {
        auto entry = model.data(model.index(0, 0), Rapid::Android::ContextMenuModel::DisplayRole::Entry)
                         .value<Rapid::Android::ContextMenuEntry*>();
        CHECK(entry->entryTitle == entries[0].entryTitle);
        CHECK(entry->iconSource == entries[0].iconSource);

        entry = model.data(model.index(1, 0), Rapid::Android::ContextMenuModel::DisplayRole::Entry)
                    .value<Rapid::Android::ContextMenuEntry*>();
        CHECK(entry->entryTitle == entries[1].entryTitle);
        CHECK(entry->iconSource == entries[1].iconSource);
    }
}
