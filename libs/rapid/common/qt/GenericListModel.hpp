// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_COMMON_QT_GENERICLISTMODEL_HPP
#define RAPID_COMMON_QT_GENERICLISTMODEL_HPP

#include <QAbstractListModel>
#include <QObject>

namespace Rapid::Common::Qt
{

/**
 * @brief Generic base for QAbstractListModel implementations
 *
 * @details Derived classes must implement the data function.
 *          It's possible to provide own role names without role name Qt default names are reported.
 *
 * @tparam ListType The element type of the QAbstractListModel.
 *                  The type holds the data that shall be displayed in the UI.
 */
template <typename ListType>
class GenericListModel : public QAbstractListModel
{
public:
    /**
     * Type alias for the role names container.
     */
    using Roles = QHash<qint32, QByteArray>;

    Q_DISABLE_COPY_MOVE(GenericListModel)

    /**
     * @brief Creates an instance of the @ref GenericListModel.
     *
     * @param roles Optional parameter to set custom roles for displayling data.
     * @param initialData Optional parameter to set initial data for the model.
     */
    GenericListModel(Roles roles = {}, std::vector<ListType> initialData = {})
        : mRoles{std::move(roles)}
        , mElements{std::move(initialData)}
    {
    }

    /**
     * Default constructor
     */
    ~GenericListModel() override = default;

    /**
     * @brief Gives the role names of the @ref GenericListModel<T> model
     *
     * @details If no custom role names are set the default Qt role names of the QAbstractListModel are returned.
     *
     * @return The role names of the model.
     */
    Roles roleNames() const noexcept override
    {
        if (mRoles.isEmpty()) {
            return QAbstractListModel::roleNames();
        }
        return mRoles;
    }

    /**
     * @brief Gives the number of elements in the model.
     *
     * @details The @ref index parameter is unused and the function always returns the amount of elements.
     *
     * @param index Unused parameter.
     *
     * @return The amount of elements in the model.
     */
    qint32 rowCount([[maybe_unused]] QModelIndex const& index) const noexcept override
    {
        return mElements.size();
    }

    /**
     * @brief Gives the data for the UI
     *
     * @details Pure virtual function that must be implemented in sub classes.
     *          An item can be requested with the protected @ref GenericListModel::getElement
     *
     * @param index The index for which the UI has requested the data.
     * @param role The type of the requested data.
     *
     * @return The data as QVariant or empty QVariant when no data found.
     */
    QVariant data(QModelIndex const& index, int role = ::Qt::DisplayRole) const noexcept override = 0;

    /**
     * @brief Adds an item to the end of the elements in the list provider
     *
     * @param item The item which is put to the end of items.
     *
     * @return The position of the inserted item
     */
    qsizetype insertItem(ListType const& item)
    {
        auto insertedIndex = mElements.size();
        beginInsertRows({}, insertedIndex, insertedIndex);
        mElements.push_back(item);
        endInsertRows();
        return insertedIndex;
    }

    /**
     * @brief adds an item to the provider in the place of the row parameter
     *
     * @details Places the item at the position "row" if the row is less than the length of the data.
     *          In all cases the item is apended to the end.
     *
     * @param row The position in which the item shall be placed.
     * @param item The new new item that shall be inserted at the row position
     *
     * @return The index (position) of the inserted item.
     */
    qsizetype insertItem(std::size_t row, ListType const& item)
    {
        if (static_cast<std::size_t>(row) > mElements.size()) {
            return insertItem(item);
        }
        beginInsertRows({}, row, row);
        auto const insertedIndex = mElements.insert(mElements.begin() + row, item);
        endInsertRows();
        return std::distance(mElements.begin(), insertedIndex);
    }

    /**
     * @brief Removes the item for the passed row
     *
     * @detils The item is only removed if the index is valid and for the index an item exists.
     *
     * @param row The position of the item that shall be removed.
     *
     * @return True means the item is deleted otherwise the false.
     */
    [[nodiscard]] bool removeItem(std::size_t row)
    {
        if (row < mElements.size()) {
            beginRemoveRows(QModelIndex(), row, row);
            mElements.erase(mElements.begin() + row);
            endRemoveRows();
            return true;
        }
        return false;
    }

    /**
     * @brief Removes all occurence of the passed item in the model.
     *
     * @param item The item that shall be remove from the model.
     *
     * @return True means at least one occurence got removed otherwise false.
     */
    [[nodiscard]] bool removeItem(ListType const& item)
    {
        auto rows = getRows(item);
        if (rows.empty()) {
            return false;
        }
        for (auto const& row : rows) {
            std::ignore = removeItem(row);
        }
        return true;
    }

    /**
     * @brief Updates the item for the passed row with the passed item.
     *
     * @details The item is only updated when the passed row parameter is a valid for the elements in the model.
     *
     * @param row The row in the elements of the list model that shall be updated.
     * @param item The new value for the item in the elements of the model.
     *
     * @return True item successful updated otherwise false.
     */
    [[nodiscard]] bool updateItem(std::size_t row, ListType const& item)
    {
        if (row < mElements.size()) {
            mElements[row] = item;
            Q_EMIT dataChanged(index(row), index(row));
            return true;
        }
        return false;
    }

    /**
     * @brief Updates all occurence of the old item with the new item
     *
     * @details If not all occurence are upated or the old item is not found the function will return false.
     *
     * @param oldItem The old item that shall be updated.
     * @param newItem The new item that replaces the old item.
     *
     * @return True all occurence are updated otherwise false.
     */
    [[nodiscard]] bool updateItem(ListType const oldItem, ListType const newItem)
    {
        auto rows = getRows(oldItem);
        if (rows.empty()) {
            return false;
        }
        for (auto const& row : rows) {
            if (not updateItem(row, newItem)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Gives a constant pointer the element for the index.
     *
     * @details If the passed index is invalid the std::nullopt is returned.
     *
     * @return A pointer to the element for a valid index or std::nullopt for an invalid index.
     */
    std::optional<ListType const*> getElement(std::size_t index) const noexcept
    {
        if (mElements.empty() or index > mElements.size() - 1) {
            return std::nullopt;
        }
        return std::addressof(mElements[index]);
    }

private:
    std::list<std::size_t> getRows(ListType const& item)
    {
        auto rows = std::list<std::size_t>{};
        for (std::size_t i = 0; i < mElements.size(); ++i) {
            if (mElements[i] == item) {
                rows.push_back(i);
            }
        }
        return rows;
    }

private:
    Roles mRoles;
    std::vector<ListType> mElements;
};

} // namespace Rapid::Common::Qt

#endif // !RAPID_COMMON_QT_GENERICLISTMODEL_HPP
