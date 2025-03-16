// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_COMMON_LAPLISTMODEL_HPP
#define RAPID_COMMON_LAPLISTMODEL_HPP

#include <common/LapData.hpp>
#include <common/qt/GenericListModel.hpp>

namespace Rapid::Common::Qt
{

/**
 * A @ref Rapid::Common::Qt::LapListModel provides access to the @ref Rapid::Common::LapData for displaying in QML in a ListView.
 */
class LapListModel : public GenericListModel<LapData>
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(LapListModel)
    /** @cond Doxygen_Suppress */
    LapListModel();
    ~LapListModel() override;
    /** @endcond */

    /**
     * @brief Creates an instance of @ref Rapid::Common::Qt::LapListModel with initial data.
     *
     * @details The laps are inserted to the model in the order of the vector.
     */
    [[nodiscard]] LapListModel(std::vector<LapData> const& laps) noexcept;

    /**
     * @brief The custom display roles supported by the model
     * @details The main purpose for the roles are for the QML context
     */
    enum DisplayRole
    {
        Laptime = ::Qt::UserRole + 1,
    };
    Q_ENUM(DisplayRole)

    /**
     * @copydoc Rapid::Common::Qt::GenericListModel
     */
    QVariant data(QModelIndex const& index, int role = ::Qt::DisplayRole) const noexcept override;
};

} // namespace Rapid::Common::Qt

#endif // !RAPID_COMMON_LAPLISTMODEL_HPP
