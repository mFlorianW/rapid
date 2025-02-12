// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceSettingsListModel.hpp"

namespace Rapid::Common::Qt
{

DeviceSettingsListModel::DeviceSettingsListModel()
    : GenericListModel<DeviceSettings>{
          QHash<int, QByteArray>{{::Qt::DisplayRole, "display"}, {DeviceSettingsListModel::Laptimer, "laptimer"}}}
{
}

DeviceSettingsListModel::~DeviceSettingsListModel() = default;

QVariant DeviceSettingsListModel::data(QModelIndex const& index, qint32 role) const noexcept
{
    auto element = getElement(index.row());
    if (not element.has_value()) {
        return {};
    }
    if (role == ::Qt::DisplayRole) {
        return (*element)->name;
    } else if (role == Laptimer) {
        return QVariant::fromValue(*element.value());
    }
    return {};
}

} // namespace Rapid::Common::Qt
