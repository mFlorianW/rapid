// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TABLEMODELDATAPROVIDER_HPP
#define TABLEMODELDATAPROVIDER_HPP

#include <QString>
#include <QStringList>
#include <QVariant>
#include <vector>

namespace Rapid::Common::Qt
{

/**
 * Helper class for defining signals for the templated @ref TableModelDataProvider.
 */
class TableModelDataProviderBase : public QObject
{
    Q_OBJECT
public:
    /**
     * Default destructor
     */
    ~TableModelDataProviderBase() override = default;

    Q_DISABLE_COPY_MOVE(TableModelDataProviderBase);
Q_SIGNALS:
    /**
     * @brief This signal is emitted when the a new item is inserted in the @ref TableModelDataProvider
     * @param index The index of the inserted item.
     */
    void beginInsertItem(qsizetype row);

    /**
     * @brief This signal is emitted when the item insertion is done.
     */
    void endInsertItem();

    /**
     * @brief This signal is emitted when an item in the @ref TableModelDataProvider is removed.
     *
     * @details So the @ref GenericTableModel use it to update the UI correctly.
     *
     * @param row The row which got removed from the model.
     */
    void beginRemoveItem(qsizetype row);

    /**
     * @brief This signal is emitted when the remove operation of an item in the @ref TableModelDataProvider is done.
     */
    void endRemoveItem();

    /**
     * @brief Emitted when an item in the @ref TableModelDataProvider is updated.
     *
     * @details This signal is used to update the UI without adding or removing an item.
     *
     * @param row The row that is updated.
     */
    void itemUpdated(qsizetype row);

protected:
    /**
     * Creates an instance of the TableModelDataProviderBase
     */
    TableModelDataProviderBase() = default;
};

/**
 * The TableModelDataProvider is data provider for a GenericTableModel.
 * A GenericTableModel uses the @TableModelDataProvider::data function to request the data.
 * Before the @ref TableModelDataProvider::data can be called a @ref TableModelDataProvider::Extractor must be set.
 * The @ref TableModelDataProvider::Extractor is used to request the data from different types.
 */
template <typename T>
class TableModelDataProvider : public TableModelDataProviderBase
{
public:
    /**
     * Alias for the extractor function that is called by the @ref TableModelDataProvider when data is requested.
     */
    using Extractor = std::function<QVariant(T const&, qsizetype, qint32)>;

    /**
     * @brief Creates an instance of the TableModelDataProvider
     * @param columNames The names of the colunmes the table model shall have.
     * @param initialData A vector with the intial items for the  @ref TableModelDataProvider of type T
     */
    explicit TableModelDataProvider(QStringList columNames, std::vector<T> initialData = {})
        : mData{std::move(initialData)}
        , mColumnNames{std::move(columNames)}
    {
    }

    /**
     * @brief Gives the amount of columns for table model.
     * @return The amount of columns
     */
    [[nodiscard]] qsizetype getColumnCount() const noexcept
    {
        return mColumnNames.size();
    }

    /**
     * @brief Gives the amount of columns for table model.
     * @return The names of the columns
     */
    [[nodiscard]] QStringList getColumnNames() const noexcept
    {
        return mColumnNames;
    }

    /**
     * Gives the amount of items in the data provider.
     */
    [[nodiscard]] qsizetype getRowCount() const noexcept
    {
        return mData.size();
    }

    /**
     * @brief Gives data elements requested by the row, column and role.
     *
     * @details This function calls the @ref Extractor to request the data of the item in the @ref TableModelDataProvider.
     *
     * @param row The index of the row for which the data is requested.
     * @param column The index of the column for which the data is requested.
     * @param role The role defines which type of data is requested.
     *             E.g. the Qt::Display role is requested when value for displaying shall be returned.
     */
    [[nodiscard]] QVariant data(qsizetype row, qsizetype column, qint32 role) const noexcept
    {
        if (row >= static_cast<qsizetype>(mData.size()) or column >= mColumnNames.size()) {
            return {};
        }
        if (not mExtractor) {
            return {};
        }
        auto const& item = mData[row];
        return mExtractor(item, column, role);
    }

    /**
     * @brief Sets a new extractor
     *
     * @details The passed function is called within the @ref TableModelDataProvider::data function to return the data.
     */
    void setDataExtractor(Extractor extractor)
    {
        mExtractor = std::move(extractor);
    }

    /**
     * @brief Adds an item to the end of the items in the data provider
     * @param item The item which is put to the end of items.
     */
    qsizetype addItem(T const& item)
    {
        auto insertedIndex = mData.size();
        beginInsertItem(insertedIndex);
        mData.push_back(item);
        endInsertItem();
        return insertedIndex;
    }

    /**
     * @brief adds an item to the provider in the place of the row parameter
     *
     * @details Places the item at the position "row" if the row is less than the length of the data.
     *          In all cases the item is apended to the end.
     *
     * @param row The position in which the item shall be placed.
     * @param item The item that shall be placed.
     */
    void addItemAt(qsizetype row, T const& item)
    {
        if (row >= static_cast<qsizetype>(mData.size())) {
            addItem(item);
            return;
        }
        beginInsertItem(row);
        mData.insert(mData.begin() + row, item);
        endInsertItem();
    }

    /**
     * @brief Gives the item at row.
     *
     * @param row The position of the item.
     *
     * @retrun The item or a std::nullopt if no item is found.
     */
    [[nodiscard]] std::optional<T> getItem(qsizetype row)
    {
        if (row < static_cast<qsizetype>(mData.size())) {
            return mData.at(row);
        }
        return std::nullopt;
    }

    /**
     * @brief Removes an item for the passed row
     *
     * @detils The item is only removed if the index is valid and for the index an item exists.
     *         The signal @ref beginRemoveItem is emitted on deletion begin.
     *         The signal @ref endRemoveItem is emitted when finished.
     *         The signals are used by the GenericTableModel to correctly update the view.
     *
     * @return True means the item is deleted otherwise the false.
     */
    [[nodiscard]] bool removeItemAt(qsizetype row)
    {
        if (row < static_cast<qsizetype>(mData.size())) {
            Q_EMIT beginRemoveItem(row);
            mData.erase(mData.begin() + row);
            Q_EMIT endRemoveItem();
            return true;
        }
        return false;
    }

    void updateItem(qsizetype row, T const& item)
    {
        if (row < static_cast<qsizetype>(mData.size())) {
            mData[row] = item;
            Q_EMIT itemUpdated(row);
        }
    }

protected:
    std::vector<T> mData; // TODO use a list to avoid reallocation/copies on insert

private:
    QStringList mColumnNames;
    Extractor mExtractor;
};

} // namespace Rapid::Common::Qt

#endif // !TABLEMODELDATAPROVIDER_HPP
