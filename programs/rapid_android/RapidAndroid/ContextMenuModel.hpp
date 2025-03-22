// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_ANDROID_CONTEXTMENUMODEL_HPP
#define RAPID_ANDROID_CONTEXTMENUMODEL_HPP

#include <common/qt/GenericListModel.hpp>

namespace Rapid::Android
{

/**
 * @brief Defines the parameter of one entry in context menu model
 */
struct ContextMenuEntry
{
    Q_GADGET

    /**
     * @property string
     *
     * Gives the title of entry.
     */
    Q_PROPERTY(QString entryTitle MEMBER entryTitle CONSTANT)

    /**
     * @property string
     *
     * Gives the optional icon source of entry.
     */
    Q_PROPERTY(QString iconSource MEMBER iconSource CONSTANT)

public:
    /**
     * @brief Default constructor
     */
    ContextMenuEntry();

    /**
     * Creates an instance of the @ref Rapid::Android::ContextMenuEntry
     *
     * @param title The title of the entry
     * @param iconSource The icon source of the entry, this an optional field and can be empty
     */
    explicit ContextMenuEntry(QString title, QString iconSource = {});

    /**
     * @brief The entry of the title
     */
    QString const entryTitle;

    /**
     * @brief The optional icon source of the entry
     */
    QString const iconSource;
};

/**
 * @brief A model that can be used in the QML ContextMenu control
 *
 * @details The model contains all the entry information for the ContextMenu control
 */
class ContextMenuModel : public Common::Qt::GenericListModel<ContextMenuEntry>
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(ContextMenuModel)

    /**
     * Custom defined roles of the model
     */
    enum DisplayRole
    {
        Entry = Qt::UserRole + 1,
    };
    Q_ENUM(DisplayRole)

    /**
     * Creates an instance of the @ref Rapid::Android::ContextMenuModel
     *
     * @param entries The entries of the context menu
     */
    ContextMenuModel(std::vector<ContextMenuEntry> entries = {});

    /** @cond Doxygen_Suppress */
    ~ContextMenuModel() override;
    /** @endcond */

    /**
     * @copydoc Rapid::Common::Qt::GenericListModel::data
     */
    QVariant data(QModelIndex const& index, int role) const noexcept override;
};

} // namespace Rapid::Android

#endif // !RAPID_ANDROID_CONTEXTMENUMODEL_HPP
