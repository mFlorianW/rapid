// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QSignalSpy>
#include <QtTestHelper.hpp>
#include <RapidApplication.hpp>
#include <TableModelDataProvider.hpp>
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

using namespace Rapid::Common::Qt;

namespace
{

struct PersonData
{
    QString name;
    quint8 age;
};

struct CountryData
{
    QString name;
    QString continent;
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

QT_CATCH2_TEST_MAIN();
