// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QAbstractItemModelTester>
#include <QModelIndex>
#include <catch2/catch_all.hpp>
#include <common/qt/GenericListModel.hpp>

using namespace Rapid::Common::Qt;

namespace
{

struct TestType
{
    qint32 value = 0;

    bool operator==(TestType const& rhs) const = default;
    bool operator!=(TestType const& rhs) const = default;
};

class TestModel : public GenericListModel<TestType>
{
    Q_OBJECT
public:
    enum Role
    {
        Role1 = ::Qt::UserRole + 1,
    };

    TestModel(std::vector<TestType> const& initialData = {})
        : GenericListModel<TestType>{getTestRoles(), initialData}
    {
    }

    ~TestModel() override = default;

    Q_DISABLE_COPY_MOVE(TestModel)

    Roles getTestRoles() const noexcept
    {
        return {{static_cast<qint32>(Role::Role1), "role1"}};
    }

    QVariant data(QModelIndex const& index, int role = ::Qt::DisplayRole) const noexcept override
    {
        if (not index.isValid()) {
            return {};
        }

        auto maybeElement = getElement(index.row());
        if (role == static_cast<int>(Role1) and maybeElement.has_value()) {
            return maybeElement.value()->value;
        }
        return {};
    }
};

class DefaultRoleNameModel : public GenericListModel<TestType>
{
    Q_OBJECT
public:
    QVariant data(QModelIndex const& index, int role = ::Qt::DisplayRole) const noexcept override
    {
        return {};
    }

    Roles getDefaultRoles()
    {
        return {{Qt::DisplayRole, "display"},
                {Qt::DecorationRole, "decoration"},
                {Qt::EditRole, "edit"},
                {Qt::ToolTipRole, "toolTip"},
                {Qt::StatusTipRole, "statusTip"},
                {Qt::WhatsThisRole, "whatsThis"}};
    }
};

struct TestFixture
{
    std::vector<TestType> data{TestType{.value = 1}, TestType{.value = 2}};
};

} // namespace

TEST_CASE_METHOD(TestFixture, "The GenericListModel shall provide the configured role names")
{
    SECTION("Custom role names")
    {
        auto model = TestModel();
        REQUIRE(model.roleNames() == model.getTestRoles());
    }

    SECTION("Default role names")
    {
        auto model = DefaultRoleNameModel();
        REQUIRE(model.roleNames() == model.getDefaultRoles());
    }
}

TEST_CASE_METHOD(TestFixture, "The GenericListModel shall provide the amount of elements in the model")
{
    auto model = TestModel(data);
    REQUIRE(model.rowCount(QModelIndex{}) == 2);
}

TEST_CASE_METHOD(TestFixture, "The GenericListModel shall provide the data in the model")
{
    auto model = TestModel(data);
    CHECK(model.data(model.index(0), TestModel::Role1).value<qint32>() == 1);
    CHECK(model.data(model.index(1), TestModel::Role1).value<qint32>() == 2);
}

TEST_CASE_METHOD(TestFixture, "The GenericListModel shall support insert of new items")
{
    auto model = TestModel(data);
    auto modelTest = QAbstractItemModelTester(&model, QAbstractItemModelTester::FailureReportingMode::Fatal);

    SECTION("Insert at the end")
    {
        CHECK(model.insertItem(TestType{.value = 3}) == 2);
        CHECK(model.data(model.index(2), TestModel::Role1).value<qint32>() == 3);
    }

    SECTION("Insert at a position smaller than elements in the model")
    {
        CHECK(model.insertItem(1, TestType{.value = 4}) == 1);
        CHECK(model.data(model.index(1), TestModel::Role1).value<qint32>() == 4);
    }

    SECTION("Insert at a position bigger than elements in the model")
    {
        CHECK(model.insertItem(100, TestType{.value = 4}) == 2);
        CHECK(model.data(model.index(2), TestModel::Role1).value<qint32>() == 4);
    }
}

TEST_CASE_METHOD(TestFixture, "The GenericListModel shall support delete of exsisting items")
{
    auto model = TestModel(data);
    auto modelTest = QAbstractItemModelTester(&model, QAbstractItemModelTester::FailureReportingMode::Fatal);

    SECTION("Delete a valid item by row")
    {
        CHECK(model.rowCount(QModelIndex{}) == 2);
        CHECK(model.removeItem(0));
        CHECK(model.rowCount(QModelIndex{}) == 1);
    }

    SECTION("Delete a invalid item by row")
    {
        CHECK(model.rowCount(QModelIndex{}) == 2);
        CHECK_FALSE(model.removeItem(100));
        CHECK(model.rowCount(QModelIndex{}) == 2);
    }

    SECTION("Delete a valid item by item")
    {
        CHECK(model.rowCount(QModelIndex{}) == 2);
        CHECK(model.removeItem(TestType{.value = 1}));
        CHECK(model.rowCount(QModelIndex{}) == 1);
    }

    SECTION("Delete an invalid item by item")
    {
        CHECK(model.rowCount(QModelIndex{}) == 2);
        CHECK_FALSE(model.removeItem(TestType{.value = 100}));
        CHECK(model.rowCount(QModelIndex{}) == 2);
    }
}

TEST_CASE_METHOD(TestFixture, "The GenericListModel shall support the update of a exsisting item")
{
    auto model = TestModel{data};
    auto modelTest = QAbstractItemModelTester(&model, QAbstractItemModelTester::FailureReportingMode::Fatal);

    SECTION("Update a valid item by row")
    {
        CHECK(model.updateItem(1, TestType{.value = 10}));
        CHECK(model.data(model.index(1), TestModel::Role1).value<qint32>() == 10);
    }

    SECTION("Update an invalid item by row")
    {
        CHECK_FALSE(model.updateItem(100, TestType{.value = 10}));
        CHECK(model.data(model.index(1), TestModel::Role1).value<qint32>() == 2);
    }

    SECTION("Update a valid item by item")
    {
        CHECK(model.updateItem(TestType{.value = 1}, TestType{.value = 10}));
        CHECK(model.data(model.index(0), TestModel::Role1).value<qint32>() == 10);
    }

    SECTION("Update an invalid item by item")
    {
        CHECK_FALSE(model.updateItem(TestType{.value = 100}, TestType{.value = 10}));
        CHECK(model.data(model.index(0), TestModel::Role1).value<qint32>() == 1);
    }
}

#include "test_qt_GenericListModel.moc"
