// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <GenericTableModel.hpp>
#include <QAbstractItemModelTester>
#include <TableModelDataProvider.hpp>
#include <catch2/catch_all.hpp>

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
