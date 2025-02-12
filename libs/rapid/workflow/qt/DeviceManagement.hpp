// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_QT_DEVICEMANAGEMENT
#define RAPID_WORKFLOW_QT_DEVICEMANAGEMENT

#include <common/qt/DeviceSettingsListModel.hpp>
#include <common/qt/DeviceSettingsProvider.hpp>
#include <common/qt/GlobalSettingsBackend.hpp>
#include <common/qt/GlobalSettingsReader.hpp>
#include <workflow/qt/IDeviceManagement.hpp>

namespace Rapid::Common::Qt::Private
{
class GlobalSettingsWriter;
}

namespace Rapid::Workflow::Qt
{

/**
 * @brief DeviceManagement impelmentation with QSettings backend.
 *
 * @details Every application should have only one instance of the DeviceManagement because the current impelmentation doesn't support multiple instances.
 */
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
    [[nodiscard]] QAbstractItemModel const* const getModel() const noexcept override;

    /**
     * @copydoc IDeviceManagement::getActiveLaptimer
     */
    [[nodiscard]] Rapid::Common::Qt::DeviceSettings getActiveLaptimer() const noexcept override;

    /**
     * @copydoc IDeviceManagement::store
     */
    Q_INVOKABLE bool store(Rapid::Common::Qt::DeviceSettings const& device) noexcept override;

    /**
     * @copydoc IDeviceManagement::update
     */
    Q_INVOKABLE bool remove(Rapid::Common::Qt::DeviceSettings const& device) noexcept override;

    /**
     * @copydoc IDeviceManagement::update
     */
    Q_INVOKABLE bool update(Rapid::Common::Qt::DeviceSettings const& oldDevice,
                            Rapid::Common::Qt::DeviceSettings const& newDevice) noexcept override;

    /**
     * @copydoc IDeviceManagement::enable
     */
    Q_INVOKABLE bool enable(Rapid::Common::Qt::DeviceSettings device) noexcept;

private:
    Common::Qt::GlobalSettingsBackend* mSettingsBackend{nullptr};
    std::unique_ptr<Common::Qt::GlobalSettingsReader> mGlobalSettingsReader;
    std::unique_ptr<Common::Qt::Private::GlobalSettingsWriter> mGlobalSettingsWriter;
    std::unique_ptr<Common::Qt::DeviceSettingsProvider> mDeviceSettingsProvider;
    std::unique_ptr<Common::Qt::DeviceSettingsListModel> mModel;
};

} // namespace Rapid::Workflow::Qt

#endif // !RAPID_WORKFLOW_QT_DEVICEMANAGEMENT
