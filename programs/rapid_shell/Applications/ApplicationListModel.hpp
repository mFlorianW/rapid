// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "ApplicationModel.hpp"
#include <QAbstractListModel>

namespace Rapid::RapidShell
{

class ApplicationListModel final : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ApplicationListModel)
public:
    enum DisplayRole
    {
        AppName = Qt::DisplayRole + 1,
        IconUrl,
        ExePath,
    };
    Q_ENUM(DisplayRole)

    ApplicationListModel(ApplicationModel& appModel) noexcept;
    ~ApplicationListModel() noexcept override = default;

    qint32 rowCount(QModelIndex const& parent) const noexcept override;
    QVariant data(QModelIndex const& index, qint32 displayRole) const noexcept override;
    QHash<qint32, QByteArray> roleNames() const noexcept override;

private:
    ApplicationModel& mAppModel;
};
} // namespace Rapid::RapidShell
