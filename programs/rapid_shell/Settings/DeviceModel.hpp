// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <GlobalSettingsWriter.hpp>
#include <QAbstractTableModel>
#include <common/qt/GlobalSettingsReader.hpp>
#include <common/qt/GlobalSettingsTypes.hpp>

namespace Rapid::RapidShell::Settings
{

/**
 * The device models holds a stored devices
 * Adds new devices and handles the changes of devices.
 */
class DeviceModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
     * Creates an instance of @ref DeviceModel.
     * @param globalSettingsWriter Used to store the device settings in the global settings.
     * @param GlobalSettingsReader Used to read the device settings on construction. The Reader is not longer needed as
     * the constructor is executed.
     *
     * @note
     * The class doesn't take the ownership of the passed pointers. So the caller must guarantee the classes
     * live as long as the @ref DeviceModel
     */
    DeviceModel(GlobalSettingsWriter* settingsWriter, Common::Qt::GlobalSettingsReader* settingsReader);

    /**
     * Default destructor
     */
    ~DeviceModel() override;

    /**
     * Disabled copy and move semantic
     */
    Q_DISABLE_COPY_MOVE(DeviceModel)

    /**
     * Gives the number of devices in the model.
     * @param parent The model index is unused.
     * @return The amount devices in model.
     */
    int rowCount(QModelIndex const& parent) const noexcept override;

    /**
     * Gives the number of column for the view.
     * The model has table has 4 columns:
     * 1. Device Name
     * 2. Device IP-Address
     * 3. Device Port
     * 4. Default Device Option (currently deactivated)
     * @param paran The parent model index is unused.
     * @return The amout of columns. This is static and is always 4.
     */
    int columnCount(QModelIndex const& parent) const noexcept override;

    /**
     * Gives the flags for the index.
     * The flags define behavior of the item behind the index.
     * This case it's all selectable, enabled and editable.
     * @param index The of the item in the model.
     * @return The flags for the item.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const noexcept override;

    /**
     * Gives the data in side model depending of the index and role.
     * @param index The index of the device in the model
     * @param role The role defines which device parameter is returned
     * @return The device data defined by the index and role.
     */
    QVariant data(QModelIndex const& index, qint32 role) const noexcept override;

    /**
     * Edits the data in the model depending of the index and role.
     * @param index The index of the device setting in the model which shall be changed.
     * @param role The role of the setting that shall be changed the function only accepts @ref Qt::DisplayRole
     * @return True The device setting is correctly changed.
     * @return False Failed to change settings. E.g invalid IP address passed.
     */
    bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole) noexcept override;

    /**
     * Gives the titles for the table header.
     * Only the Qt::DisplayRole is supported
     * @param index The table column header index.
     * @param orientation The of the header only Qt::Horizontal is supported.
     * @return The header data for the column.
     */
    QVariant headerData(qint32 index, Qt::Orientation orientation, qint32 role) const noexcept override;

    /**
     * Creates a device setting in the model with default parameters.
     * The function can only append new settings to the model.
     * That means the @param row must be equal to @ref DeviceModel::rowCount.
     * @param row At which row shall the new device settings created.
     * @param count How many device settings shall be created, default 1.
     * @param parent In this case unused.
     * @return true The new devices are created.
     * @return false Failed to create the new devices.
     */
    bool insertRows(int row, int count = 1, QModelIndex const& parent = QModelIndex{}) noexcept override;

    /**
     * Removes device settings in the model.
     * @param row At which row shall the device settings removed. This value acts as poistion in the Model.
     * @param count How many device settings shall be removed, default 1.
     * @param parent In this case unused.
     * @return true The new devices are removed.
     * @return false Failed to remove the device.
     */
    bool removeRows(int row, int count = 1, QModelIndex const& parent = QModelIndex{}) noexcept override;

    /**
     * Stores the current device settings in the global settings
     * @return true The device settings are saved.
     * @return false Failed to save the device settings.
     */
    bool save() noexcept;

    /**
     * Restores the original device settings which was loaded on default or saved.
     * @return true Successful restored
     * @return false Failed to restore
     */
    bool restore() noexcept;

private:
    bool isInvalidIndex(QModelIndex const& index) const noexcept;
    bool setIpAddress(Common::Qt::DeviceSettings& device, QVariant const& rawIp) noexcept;
    bool setDevicePort(Common::Qt::DeviceSettings& device, QVariant const& rawPort) noexcept;

    void backup() noexcept;

    QList<Common::Qt::DeviceSettings> mDevices;
    QList<Common::Qt::DeviceSettings> mOriginalDeviceSettings;
    GlobalSettingsWriter* mSettingsWriter = nullptr;
    bool mBackuped = false;
};

} // namespace Rapid::RapidShell::Settings
