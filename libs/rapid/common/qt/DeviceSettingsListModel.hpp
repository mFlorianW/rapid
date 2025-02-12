// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_COMMON_QT_DEVCIESETTINGSLISTMODEL_HPP
#define RAPID_COMMON_QT_DEVCIESETTINGSLISTMODEL_HPP

#include <common/qt/GenericListModel.hpp>
#include <common/qt/GlobalSettingsTypes.hpp>

namespace Rapid::Common::Qt
{

/**
 * @brief A model for showing the @ref DeviceSettings as list in the UI.
 *
 * @details The model can be used in different contexts.
 *          In the widget context the model will only return the name of the device requested by the Qt::DisplayRole.
 *          When used in the QML context the model define three new roles:
 *              laptimer: To request the laptimer settings
 */
class DeviceSettingsListModel : public GenericListModel<DeviceSettings>
{
    Q_OBJECT
public:
    /**
     * @brief The custom display roles supported by the model
     * @details The main purpose for the roles are for the QML context
     */
    enum DisplayRole
    {
        Laptimer = ::Qt::UserRole + 1,
    };
    Q_ENUM(DisplayRole)

    /**
     * @brief Creates an instance of the @ref DeviceSettingsListModel
     */
    [[nodiscard]] DeviceSettingsListModel();

    /**
     * Default destructor
     */
    ~DeviceSettingsListModel() override;

    Q_DISABLE_COPY_MOVE(DeviceSettingsListModel)

    /**
     * @copydoc GenericListModel::data
     */
    [[nodiscard]] QVariant data(QModelIndex const& index, qint32 role = ::Qt::DisplayRole) const noexcept override;
};

} // namespace Rapid::Common::Qt

#endif // !RAPID_COMMON_QT_DEVCIESETTINGSLISTMODEL_HPP
