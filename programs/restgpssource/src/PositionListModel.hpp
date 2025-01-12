// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef POSITIONLISTMODEL_HPP
#define POSITIONLISTMODEL_HPP

#include <QAbstractListModel>
#include <memory>

namespace Rapid::Common
{
class GpsPositionData;
}

class PositionListModelPrivate;
class PositionListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PositionListModel)
public:
    enum PositionRole
    {
        Latitude = Qt::DisplayRole + 1,
        Longitude,
    };
    Q_ENUM(PositionRole)

    PositionListModel();
    ~PositionListModel() override;

    qint32 rowCount(QModelIndex const& index) const override;

    QVariant data(QModelIndex const& index, qint32 role) const override;

    QHash<qint32, QByteArray> roleNames() const override;

    void addPosition(Rapid::Common::GpsPositionData const& position);

    void clear();

private:
    std::unique_ptr<PositionListModelPrivate> d;
};

#endif // !POSITIONLISTMODEL_HPP
