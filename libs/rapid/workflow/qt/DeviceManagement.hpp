// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_QT_DEVICEMANAGEMENT
#define RAPID_WORKFLOW_QT_DEVICEMANAGEMENT

#include <QAbstractItemModel>
#include <common/qt/DeviceSettingsProvider.hpp>
#include <common/qt/GlobalSettingsBackend.hpp>
#include <common/qt/GlobalSettingsReader.hpp>
#include <workflow/qt/IDeviceManagement.hpp>

namespace Rapid::Workflow::Qt
{

class DeviceManagement : public IDeviceManagement
{
    Q_OBJECT

public:
    /**
     * Creates an instnace of DeviceManagement
     *
     */
    DeviceManagement(Common::Qt::GlobalSettingsBackend* settingsBackend);

    /**
     * Default destructor
     */
    ~DeviceManagement() override;

    /**
     * Disabled copy constructor
     */
    DeviceManagement(DeviceManagement const&) = delete;

    /**
     * Disablbed copy assignment operator
     */
    DeviceManagement& operator=(DeviceManagement const&) = delete;

    /**
     * Disabled move constructor
     */
    DeviceManagement(DeviceManagement&&) noexcept = delete;

    /**
     * Disablbed move assignment operator
     */
    DeviceManagement& operator=(DeviceManagement&&) noexcept = delete;

    /**
     * @copydoc IDeviceManagement::getModel
     */
    QAbstractItemModel const* const getModel() const noexcept;

private:
    std::unique_ptr<Common::Qt::GlobalSettingsReader> mGlobalSettingsReader;
    std::unique_ptr<Common::Qt::DeviceSettingsProvider> mDeviceSettingsProvider;
    std::unique_ptr<QAbstractItemModel> mModel;
};

} // namespace Rapid::Workflow::Qt

#endif // !RAPID_WORKFLOW_QT_DEVICEMANAGEMENT
