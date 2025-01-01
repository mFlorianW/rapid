// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QSignalSpy>
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <common/qt/TableModelDataProvider.hpp>

using namespace Rapid::Common::Qt;

namespace
{

struct PersonData
{
    QString name;
    quint8 age;

    bool operator==(PersonData const& rhs) const = default;
};

struct CountryData
{
    QString name;
    QString continent;

    bool operator==(CountryData const& rhs) const = default;
};

} // namespace

TEST_CASE("the TableModelDataProvider shall provide the columnames and columncount")
{
    auto dataProvider = TableModelDataProvider<PersonData>{{"1", "2"}, {PersonData{.name = QString{"Name"}, .age = 8}}};
    auto expectedColumnNames = QStringList{"1", "2"};
    auto columnNames = dataProvider.getColumnNames();
    auto columnCount = dataProvider.getColumnCount();
    CHECK(columnCount == expectedColumnNames.size());
    REQUIRE(columnNames == expectedColumnNames);
}

TEST_CASE("the TableModelDataProvider shall provide a generic way to request data with a row and column index")
{
    auto dataProvider1 =
        TableModelDataProvider<PersonData>{{"1", "2"}, {PersonData{.name = QString{"Name"}, .age = 8}}};
    auto dataProvider2 =
        TableModelDataProvider<CountryData>{{"1", "2"},
                                            {CountryData{.name = QString{"Name"}, .continent = "Continent"}}};
    dataProvider1.setDataExtractor([](auto&& item, qsizetype column, qint32) -> QVariant {
        switch (column) {
        case 0:
            return item.name;
        case 1:
            return item.age;
        default:
            return {};
        }
    });

    dataProvider2.setDataExtractor([](auto&& item, qsizetype column, qint32) -> QVariant {
        switch (column) {
        case 0:
            return item.name;
        case 1:
            return item.continent;
        default:
            return {};
        }
    });

    auto const expectedName = QStringLiteral("Name");
    constexpr auto expectedAge = quint8{8};
    auto name = dataProvider1.data(0, 0, Qt::DisplayRole);
    CHECK(name.toString() == expectedName);
    auto age = dataProvider1.data(0, 1, Qt::DisplayRole);
    CHECK(age.toUInt() == expectedAge);

    auto const expectedContinent = QStringLiteral("Continent");
    name = dataProvider2.data(0, 0, Qt::DisplayRole);
    CHECK(name.toString() == expectedName);
    auto continent = dataProvider2.data(0, 1, Qt::DisplayRole);
    REQUIRE(continent.toString() == expectedContinent);
}

TEST_CASE("The TableModelDataProvider shall notify when a item is added.")
{
    auto dataProvider = TableModelDataProvider<PersonData>{{"1", "2"}, {PersonData{.name = QString{"Name"}, .age = 8}}};
    auto beginInsertItemSpy = QSignalSpy{&dataProvider, &TableModelDataProvider<PersonData>::beginInsertItem};
    auto endInsertItemSpy = QSignalSpy{&dataProvider, &TableModelDataProvider<PersonData>::endInsertItem};

    auto insertedIndex = dataProvider.addItem(PersonData{.name = "Welt", .age = 12});
    REQUIRE(beginInsertItemSpy.size() == 1);
    CHECK(beginInsertItemSpy.first().first().value<qsizetype>() == 1);
    REQUIRE(endInsertItemSpy.size() == 1);
    CHECK(insertedIndex == 1);
    REQUIRE(dataProvider.getRowCount() == 2);
}

TEST_CASE("The TableModelDataProvider shall provide the item for an index")
{
    auto dataProvider = TableModelDataProvider<PersonData>{
        {"1", "2"},
        {PersonData{.name = QString{"Name"}, .age = 8}, PersonData{.name = QString{"Name1"}, .age = 9}}};
    SECTION("Valid index")
    {
        auto item = dataProvider.getItem(0);
        REQUIRE(item.value_or(PersonData{}) == PersonData{.name = QString{"Name"}, .age = 8});
        item = dataProvider.getItem(1);
        REQUIRE(item.value_or(PersonData{}) == PersonData{.name = QString{"Name1"}, .age = 9});
    }

    SECTION("Invalid index")
    {
        auto item = dataProvider.getItem(100);
        REQUIRE(item == std::nullopt);
    }
}

TEST_CASE("The TableModelDataProvider shall remove an item for an index")
{
    auto dataProvider = TableModelDataProvider<PersonData>{
        {"1", "2"},
        {PersonData{.name = QString{"Name"}, .age = 8}, PersonData{.name = QString{"Name1"}, .age = 9}}};

    SECTION("Remove with valid index")
    {
        CHECK(dataProvider.removeItemAt(0));
        CHECK(dataProvider.getRowCount() == 1);
        REQUIRE(dataProvider.getItem(0).value_or(PersonData{}) ==
                PersonData{.name = QStringLiteral("Name1"), .age = 9});
    }

    SECTION("Remove with invalid index")
    {
        CHECK_FALSE(dataProvider.removeItemAt(3));
    }

    SECTION("Emits deletions signals are emitted")
    {
        auto beginRemoveItem = QSignalSpy{&dataProvider, &TableModelDataProviderBase::beginRemoveItem};
        auto endRemoveItem = QSignalSpy{&dataProvider, &TableModelDataProviderBase::endRemoveItem};

        CHECK(dataProvider.removeItemAt(1));

        REQUIRE(beginRemoveItem.size() == 1);
        REQUIRE(endRemoveItem.size() == 1);
        REQUIRE(beginRemoveItem.at(0).at(0).value<qsizetype>() == 1);
    }
}

TEST_CASE("The TableModelDataProvider shall support ordered inserts")
{
    auto const person1 = PersonData{.name = QString{"Name"}, .age = 8};
    auto const person2 = PersonData{.name = QString{"Name1"}, .age = 9};
    auto dataProvider = TableModelDataProvider<PersonData>{{"1", "2"}, {person1, person2}};
    auto const insertItem = PersonData{.name = "hello", .age = 10};

    auto beginInsertItem = QSignalSpy{&dataProvider, &TableModelDataProviderBase::beginInsertItem};
    auto endInsertItem = QSignalSpy{&dataProvider, &TableModelDataProviderBase::endInsertItem};

    SECTION("Insert item at the end")
    {
        dataProvider.addItemAt(dataProvider.getRowCount(), insertItem);
        REQUIRE(dataProvider.getRowCount() == 3);
        REQUIRE(dataProvider.getItem(2) == insertItem);

        REQUIRE(beginInsertItem.size() == 1);
        REQUIRE(endInsertItem.size() == 1);
        REQUIRE(beginInsertItem.at(0).at(0).value<qsizetype>() == 2);
    }

    SECTION("Insert existing item and moves the other")
    {
        dataProvider.addItemAt(1, insertItem);
        REQUIRE(dataProvider.getRowCount() == 3);
        REQUIRE(dataProvider.getItem(1) == insertItem);
        REQUIRE(dataProvider.getItem(2) == person2);

        REQUIRE(beginInsertItem.size() == 1);
        REQUIRE(endInsertItem.size() == 1);
        REQUIRE(beginInsertItem.at(0).at(0).value<qsizetype>() == 1);
    }
}

TEST_CASE("The TableModelDataProvider shall support item updates")
{
    auto const person1 = PersonData{.name = QString{"Name"}, .age = 8};
    auto const person2 = PersonData{.name = QString{"Name1"}, .age = 9};
    auto dataProvider = TableModelDataProvider<PersonData>{{"1", "2"}, {person1}};
    auto updateItemSpy = QSignalSpy{&dataProvider, &TableModelDataProviderBase::itemUpdated};

    SECTION("Invalid row")
    {
        dataProvider.updateItem(12345, person2);
        REQUIRE(updateItemSpy.size() == 0);
        REQUIRE(dataProvider.getItem(0) == person1);
    }

    SECTION("valid row")
    {
        dataProvider.updateItem(0, person2);
        REQUIRE(updateItemSpy.size() == 1);
        REQUIRE(dataProvider.getItem(0) == person2);
    }
}
