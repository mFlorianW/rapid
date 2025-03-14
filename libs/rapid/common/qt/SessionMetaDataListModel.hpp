// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_COMMON_QT_SESSIONMETADATALISTMODEL
#define RAPID_COMMON_QT_SESSIONMETADATALISTMODEL

#include <common/SessionMetaData.hpp>
#include <common/qt/GenericListModel.hpp>

namespace Rapid::Common::Qt
{

/**
 * A @ref SessionMetaDataListModel provides the @ref Rapid::Common::SessionMetaData for displaying in QML in a ListView.
 */
class SessionMetaDataListModel : public GenericListModel<SessionMetaData>
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(SessionMetaDataListModel)

    /**
     * @brief The custom display roles supported by the model
     * @details The main purpose for the roles are for the QML context
     */
    enum DisplayRole
    {
        TrackName = ::Qt::UserRole + 1,
        Time,
        Date
    };

    /**
     * @brief Creates an instance of the @ref SessionMetaDataListModel
     */
    [[nodiscard]] SessionMetaDataListModel();

    /**
     * Default destructor
     */
    ~SessionMetaDataListModel() override;

    /**
     * @copydoc Rapid::Common::Qt::GenericListModel
     */
    [[nodiscard]] QVariant data(QModelIndex const& index, int role = ::Qt::DisplayRole) const noexcept override;
};

} // namespace Rapid::Common::Qt

#endif // !RAPID_COMMON_QT_SESSIONMETADATALISTMODEL
