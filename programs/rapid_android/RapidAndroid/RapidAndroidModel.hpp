// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_ANDROID_RAPIDANDROIDMODEL_HPP
#define RAPID_ANDROID_RAPIDANDROIDMODEL_HPP

#include <QQmlEngine>
#include <workflow/qt/DeviceManagement.hpp>
#include <workflow/qt/LocalSessionManagement.hpp>
#include <workflow/qt/RestActiveSession.hpp>

namespace Rapid::Android
{

/**
 * @brief Defines the global state for the QML context
 *
 */
class RapidAndroidModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @property Rapid::Common::Qt::DeviceSettings
     *
     * This property the current active laptimer settings
     */
    Q_PROPERTY(Rapid::Common::Qt::DeviceSettings activeLaptimer READ getActiveLaptimer NOTIFY activeLaptimerChanged)

    /**
     * @property Rapid::Workflow::Qt::DeviceManagement*
     *
     * Gives the DeviceManagement workflow for managing the laptimer.
     */
    Q_PROPERTY(Rapid::Workflow::Qt::DeviceManagement* deviceManagement READ getDeviceManagement CONSTANT)

public:
    Q_DISABLE_COPY_MOVE(RapidAndroidModel)

    /**
     * Creates an instance of the @ref GlobalState
     */
    RapidAndroidModel();

    /**
     * Default destructor
     */
    ~RapidAndroidModel() override;

    /**
     * @brief Helper Method to create @ref DeviceSettings in the QML context
     *
     * @return The @ref Rapid::Common::Qt::DeviceSetting instance with created from the string parameters
     */
    Q_INVOKABLE static Rapid::Common::Qt::DeviceSettings create(QString const& name,
                                                                QString const& ip,
                                                                QString port,
                                                                bool enabled) noexcept;

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the active laptimer is changed.
     *
     * @details The active laptimer is changed when in the laptimer page a different laptimer is activated.
     */
    void activeLaptimerChanged();

private:
    [[nodiscard]] Rapid::Workflow::Qt::DeviceManagement* getDeviceManagement() noexcept;
    [[nodiscard]] Rapid::Common::Qt::DeviceSettings getActiveLaptimer() const noexcept;

    Rapid::Common::Qt::QSettingsBackend mSettingsBackend;
    Rapid::Workflow::Qt::DeviceManagement mDeviceManagement{&mSettingsBackend};
};

} // namespace Rapid::Android

#endif // RAPID_ANDROID_RAPIDANDROIDMODEL_HPP
