// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef GENERICTABLEMODEL_HPP
#define GENERICTABLEMODEL_HPP

#include <QAbstractTableModel>
#include <QMetaMethod>

namespace Rapid::Common::Qt
{

/**
 * Concept definition for a DataProvider for the GenericTableModel
 */
template <typename T>
concept DataProvider = requires(T t, qsizetype row, qsizetype column, qint32 role) {
    { t.getColumnCount() } -> std::same_as<qsizetype>;
    { t.getColumnNames() } -> std::same_as<QStringList>;
    { t.getRowCount() } -> std::same_as<qsizetype>;
    { t.data(row, column, role) } -> std::same_as<QVariant>;
    { t.beginInsertItem(row) } -> std::same_as<void>;
    { t.endInsertItem() } -> std::same_as<void>;
    { t.beginRemoveItem(row) } -> std::same_as<void>;
    { t.endInsertItem() } -> std::same_as<void>;
    { t.itemUpdated(row) } -> std::same_as<void>;
};

/**
 * Generic implementation of the QAbstractTableModel for the usage with different types.
 * The @ref GenericTableModel can use every DataProvider that fulfils the @ref DataProvider concept.
 * @tparam T The DataProvider which is used to request the data from.
 */
template <DataProvider T>
class GenericTableModel : public QAbstractTableModel
{
public:
    /**
     * @brief Creates an instance of the GenericTableModel
     *
     * @details The GenericTableModel doesn't take the ownership of the passed DataProvider.
     *          That means the DataProvider must have at least the same life time as @ref GenericTableModel.
     *
     * @param dataProvider The dataprovider that is used to request the data from.
     */
    GenericTableModel(T& dataProvider)
        : mDataProvider{dataProvider}
    {
        connect(&mDataProvider, &T::beginInsertItem, this, [this](auto&& row) {
            beginInsertRows(QModelIndex(), row, row);
        });

        connect(&mDataProvider, &T::endInsertItem, this, [this] {
            endInsertRows();
        });

        connect(&mDataProvider, &T::beginRemoveItem, this, [this](auto&& row) {
            beginRemoveRows(QModelIndex(), row, row);
        });

        connect(&mDataProvider, &T::endRemoveItem, this, [this] {
            endRemoveRows();
        });

        connect(&mDataProvider, &T::itemUpdated, this, [this](auto&& row) {
            Q_EMIT dataChanged(index(row, 0), index(row, mDataProvider.getColumnCount() - 1));
        });
    }

    /**
     * @brief Gives the amount columns of the table.
     *
     * @details For a valid parent index the function will always return 0.
     *
     * @param parent Not used.
     */
    qint32 columnCount([[maybe_unused]] QModelIndex const& parent = QModelIndex()) const noexcept override
    {
        // For table models the column count shall be 0 when the parent is valid.
        if (parent.isValid()) {
            return 0;
        }
        return mDataProvider.getColumnCount();
    }

    /**
     * @brief Gives the amount columns of the table.
     *
     * @details For a valid parent index the function will always return 0.
     *
     * @param parent Not used.
     */
    qint32 rowCount([[maybe_unused]] QModelIndex const& parent = QModelIndex()) const noexcept override
    {
        // For table models the row count shall be 0 when the parent is valid.
        if (parent.isValid()) {
            return 0;
        }
        return mDataProvider.getRowCount();
    }

    /**
     * @brief Gives the data for the UI.
     *
     * @details All the parameters are forwared to DataProvider which is responsible to return the correct data.
     *
     * @param index The index for which the data are requested.
     * @param role Which type of data are requested.
     * @return The requested data or in case of an error a default constructed QVariant.
     */
    QVariant data(QModelIndex const& index, int role = ::Qt::DisplayRole) const noexcept override
    {
        return mDataProvider.data(index.row(), index.column(), role);
    }

    /**
     * @brief Gives the header data for the UI.
     *
     * @details All the parameters are forwarded to the DataPovider which is responsible to return the correct data.
     * @param section The section for which the header data are requested.
     * @param orientation The orientation in which the data is going to be displayed.
     * @param role Which type of data are requested.
     * @return The requested data or in case of an error a default constructed QVaraint
     */
    QVariant headerData(int section,
                        ::Qt::Orientation orientation,
                        int role = ::Qt::DisplayRole) const noexcept override
    {
        if (section < mDataProvider.getColumnNames().size() && orientation == ::Qt::Orientation::Horizontal &&
            role == ::Qt::DisplayRole) {
            return mDataProvider.getColumnNames().at(section);
        }
        return {};
    }

private:
    T& mDataProvider;
};

} // namespace Rapid::Common::Qt

#endif // GENERICTABLEMODEL_HPP
