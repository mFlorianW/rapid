// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_COMMON_QT_SESSIONMETADATASORTLISTMODEL_HPP
#define RAPID_COMMON_QT_SESSIONMETADATASORTLISTMODEL_HPP

#include <QSortFilterProxyModel>

namespace Rapid::Common::Qt
{

/**
 * @brief Proxy sorting model for a @ref Rapid::Common::Qt::SessionMetaDataListModel
 *
 * @details All sessions metadata are ordered descending based on the date and time.
 */
class SessionMetaDataSortListModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(SessionMetaDataSortListModel)

    /** @cond Doxygen_Suppress */
    SessionMetaDataSortListModel();
    ~SessionMetaDataSortListModel() override;
    /** @endcond Doxygen_Suppress */

    /**
     * @brief Custom implementation for sorting on the date and time
     *
     * @param sourceLeft The left hand session metadata of the compare
     * @param sourceRight The right hand session metadata of the compare
     *
     * @return true The left hand session metadata is smaller than the sourceRight element
     */
    bool lessThan(QModelIndex const& sourceLeft, QModelIndex const& sourceRight) const override;
};

} // namespace Rapid::Common::Qt

#endif // RAPID_COMMON_QT_SESSIONMETADATASORTLISTMODEL_HPP
