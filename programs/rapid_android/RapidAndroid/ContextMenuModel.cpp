// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ContextMenuModel.hpp"

namespace Rapid::Android
{

ContextMenuEntry::ContextMenuEntry() = default;

ContextMenuEntry::ContextMenuEntry(QString title, QString iconSource)
    : entryTitle{std::move(title)}
    , iconSource{std::move(iconSource)}
{
}

ContextMenuModel::ContextMenuModel(std::vector<ContextMenuEntry> entries)
    : Rapid::Common::Qt::GenericListModel<ContextMenuEntry>{{{DisplayRole::Entry, {"entry"}}}, std::move(entries)}
{
}

ContextMenuModel::~ContextMenuModel() = default;

QVariant ContextMenuModel::data(QModelIndex const& index, int role) const noexcept
{
    if (not index.isValid()) {
        return {};
    }
    auto entry = getElement(index.row());
    if (role == DisplayRole::Entry and entry.has_value()) {
        return QVariant::fromValue(entry.value());
    }
    return {};
}

} // namespace Rapid::Android
