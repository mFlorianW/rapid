// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationListModel.hpp"

namespace Rapid::RapidShell
{

ApplicationListModel::ApplicationListModel(ApplicationModel& appModel) noexcept
    : QAbstractListModel{}
    , mAppModel{appModel}
{
}

qint32 ApplicationListModel::rowCount(QModelIndex const& parent) const noexcept
{
    return static_cast<qint32>(mAppModel.getApplications().size());
}

QVariant ApplicationListModel::data(QModelIndex const& index, qint32 displayRole) const noexcept
{
    auto const apps = mAppModel.getApplications();
    if (!index.isValid() or index.row() > apps.size()) {
        return {};
    }

    if (displayRole == DisplayRole::AppName) {
        return apps.at(index.row()).getName();
    } else if (displayRole == DisplayRole::IconUrl) {
        return apps.at(index.row()).getIconUrl().prepend("file:");
    } else if (displayRole == DisplayRole::ExePath) {
        return apps.at(index.row()).getExecutable();
    }
    return {};
}

QHash<qint32, QByteArray> ApplicationListModel::roleNames() const noexcept
{
    static auto roles = QHash<qint32, QByteArray>{{DisplayRole::AppName, "appName"},
                                                  {DisplayRole::IconUrl, "iconUrl"},
                                                  {DisplayRole::ExePath, "exePath"}};
    return roles;
}

} // namespace Rapid::RapidShell
