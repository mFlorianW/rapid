// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <catch2/catch_all.hpp>
#include <common/qt/GenericTableModel.hpp>
#include <common/qt/TableModelDataProvider.hpp>

using namespace Rapid::Common::Qt;

namespace
{

struct TestData
{
    qint32 data{0};
};

struct TestFixture
{
    TableModelDataProvider<TestData> dataProvider;
    GenericTableModel<TableModelDataProvider<TestData>> model;
    QAbstractItemModelTester modelTester;

    TestFixture()
        : dataProvider{{"1"}, {TestData{.data = 0x01}}}
        , model{dataProvider}
        , modelTester{&model, QAbstractItemModelTester::FailureReportingMode::Fatal}
    {
        dataProvider.setDataExtractor([](auto&& item, qsizetype column, qint32 role) -> QVariant {
            REQUIRE(column == 0);
            if (role != Qt::DisplayRole) {
                return {};
            }
            return item.data;
        });
    }
};

} // namespace

TEST_CASE_METHOD(TestFixture, "The GenericTableModel shall return the column names of the data provider")
{
    CHECK(model.columnCount() == 1);
    REQUIRE(model.headerData(0, ::Qt::Orientation::Horizontal).toString() == QStringLiteral("1"));
}

TEST_CASE_METHOD(TestFixture, "The GenericTableModel shall return the row count of the data provider")
{
    REQUIRE(model.rowCount() == 1);
}

TEST_CASE_METHOD(TestFixture, "The GenericTableModel shall return the data for displaying")
{
    REQUIRE(model.data(model.index(0, 0)).toUInt() == 0x01);
}

TEST_CASE_METHOD(TestFixture, "The GenericTableModel shall correctly notify about updates on provider changes")
{
    dataProvider.addItem(TestData{.data = 0x02});
    REQUIRE(model.data(model.index(1, 0)).toUInt() == 0x02);
}

TEST_CASE_METHOD(TestFixture, "The GenericTable shall correctly notify about deletion in the provider")
{
    SECTION("Item is removed")
    {
        REQUIRE(model.rowCount() == 1);
        REQUIRE(dataProvider.removeItemAt(0));
        REQUIRE(model.rowCount() == 0);
    }

    SECTION("Signals are correctly emitted")
    {
        auto rowsAboutToRemovedSpy = QSignalSpy{&model, &QAbstractItemModel::rowsAboutToBeRemoved};
        auto rowsRemovedSpy = QSignalSpy{&model, &QAbstractItemModel::rowsRemoved};

        REQUIRE(dataProvider.removeItemAt(0));

        REQUIRE(rowsAboutToRemovedSpy.size() == 1);
        REQUIRE(rowsAboutToRemovedSpy.at(0).at(1).toInt() == 0);
        REQUIRE(rowsAboutToRemovedSpy.at(0).at(2).toInt() == 0);
        REQUIRE(rowsRemovedSpy.size() == 1);
        REQUIRE(rowsRemovedSpy.at(0).at(1).toInt() == 0);
        REQUIRE(rowsRemovedSpy.at(0).at(2).toInt() == 0);
    }
}

TEST_CASE_METHOD(TestFixture, "The GenericTable shall correctly notify about updates in the provider")
{
    auto dataChangedSpy = QSignalSpy{&model, &QAbstractItemModel::dataChanged};

    dataProvider.updateItem(0, TestData{.data = 123});

    REQUIRE(dataChangedSpy.size() == 1);
    REQUIRE(dataChangedSpy.at(0).at(0).value<QModelIndex>().row() == 0);
    REQUIRE(dataChangedSpy.at(0).at(0).value<QModelIndex>().column() == 0);
    REQUIRE(dataChangedSpy.at(0).at(1).value<QModelIndex>().column() == 0);
    REQUIRE(dataChangedSpy.at(0).at(1).value<QModelIndex>().column() == 0);
}
