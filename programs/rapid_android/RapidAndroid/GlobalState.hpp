// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_ANDROID_GLOBALSTATE_HPP
#define RAPID_ANDROID_GLOBALSTATE_HPP

#include <QQmlEngine>
#include <workflow/qt/DeviceManagement.hpp>
#include <workflow/qt/LocalSessionManagement.hpp>
#include <workflow/qt/RestActiveSession.hpp>

namespace Rapid::Rest
{
class QRestClient;
}

namespace Rapid::Storage
{
class ISessionDatabase;
}

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

    /**
     * @property Rapid::Workflow::Qt::RestActiveSession
     *
     * Gives the REST active session for displaying the information for the active session
     */
    Q_PROPERTY(Rapid::Workflow::Qt::RestActiveSession const* activeSession READ getActiveSession CONSTANT)

    /**
     * @property Rapid::Workflow::Qt::LocalSessionManagement
     *
     * Gives the local session management
     */
    Q_PROPERTY(
        Rapid::Workflow::Qt::LocalSessionManagement* localSessionManagement READ getLocalSessionManagement CONSTANT)

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
     * @brief Gives the access to the active session.
     *
     * @details The REST client is automatically updated, when the active device is changed.
     *
     * @return A pointer to the active session.
     */
    Rapid::Workflow::Qt::RestActiveSession const* getActiveSession() const noexcept;

    /**
     * @brief Gives the local session management implementation.
     *
     * @return A pointer to the local session management.
     */
    Rapid::Workflow::Qt::LocalSessionManagement* getLocalSessionManagement() const noexcept;

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
    void setRestClient();

    Rapid::Common::Qt::QSettingsBackend mSettingsBackend;
    Rapid::Workflow::Qt::DeviceManagement* mDeviceManagement{nullptr};
    std::unique_ptr<Rest::QRestClient> mRestClient;
    std::unique_ptr<Workflow::Qt::RestActiveSession> mRestActiveSession;
    std::unique_ptr<Storage::ISessionDatabase> mSessionDatabase;
    std::unique_ptr<Workflow::Qt::LocalSessionManagement> mLocalSessionManagement;
};

} // namespace Rapid::Android

#endif // RAPID_ANDROID_GLOBALSTATE_HPP
