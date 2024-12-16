// SPDX-FileCopyrightText: 2024 All contributors
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
     * This signal is emitted when the a new item is inserted in the @ref TableModelDataProvider
     * @param index The index of the inserted item.
     */
    void beginInsertItem(qsizetype index);

    /**
     * This signal is emitted when the item insertion is done.
     */
    void endInsertItem();

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
    qsizetype getColumnCount() const noexcept
    {
        return mColumnNames.size();
    }

    /**
     * @brief Gives the amount of columns for table model.
     * @return The amount of columns
     */
    QStringList getColumnNames() const noexcept
    {
        return mColumnNames;
    }

    /**
     * Gives the amount of items in the data provider.
     */
    qsizetype getRowCount() const noexcept
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
    QVariant data(qsizetype row, qsizetype column, qint32 role) const noexcept
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
     * Adds an item to the data provider
     */
    qsizetype addItem(T const& item)
    {
        auto insertedIndex = mData.size();
        beginInsertItem(insertedIndex);
        mData.push_back(item);
        endInsertItem();
        return insertedIndex;
    }

    std::optional<T> getItem(qsizetype row)
    {
        if (row < static_cast<qsizetype>(mData.size())) {
            return mData.at(row);
        }
        return std::nullopt;
    }

protected:
    std::vector<T> mData;

private:
    QStringList mColumnNames;
    Extractor mExtractor;
};

} // namespace Rapid::Common::Qt

#endif // !TABLEMODELDATAPROVIDER_HPP
