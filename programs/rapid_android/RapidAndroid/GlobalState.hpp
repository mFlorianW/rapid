// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_ANDROID_GLOBALSTATE_HPP
#define RAPID_ANDROID_GLOBALSTATE_HPP

#include <QQmlEngine>
#include <workflow/qt/DeviceManagement.hpp>

namespace Rapid::Android
{

/**
 * @brief Defines the global state for the QML context
 *
 */
class GlobalState : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    /**
     * @property Rapid::Workflow::Qt::DeviceManagement*
     *
     * Gives the DeviceManagement workflow for managing the laptimer.
     */
    Q_PROPERTY(Rapid::Workflow::Qt::DeviceManagement* deviceManagement MEMBER mDeviceManagement CONSTANT)
public:
    Q_DISABLE_COPY_MOVE(GlobalState)

    /**
     * Creates an instance of the @ref GlobalState
     */
    GlobalState();

    /**
     * Default destructor
     */
    ~GlobalState() override;

    /**
     * @brief Helper Method to create @ref DeviceSettings in the QML context
     *
     * @return The instance with created from the string parameters
     */
    Q_INVOKABLE static Rapid::Common::Qt::DeviceSettings create(QString const& name,
                                                                QString const& ip,
                                                                QString port,
                                                                bool enabled) noexcept;

private:
    Rapid::Common::Qt::QSettingsBackend mSettingsBackend;
    Rapid::Workflow::Qt::DeviceManagement* mDeviceManagement;
};

} // namespace Rapid::Android

#endif // RAPID_ANDROID_GLOBALSTATE_HPP
