// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LapListModel.hpp"

namespace Rapid::Common::Qt
{

LapListModel::LapListModel()
    : GenericListModel<LapData>{{{DisplayRole::Laptime, "laptime"}}}
{
}

LapListModel::~LapListModel() = default;

QVariant LapListModel::data(QModelIndex const& index, int role) const noexcept
{
    auto lap = getElement(index.row());
    if (lap.has_value() and role == DisplayRole::Laptime) {
        return QString::fromStdString((*lap)->getLaptime().asString());
    }
    return {};
}

} // namespace Rapid::Common::Qt
