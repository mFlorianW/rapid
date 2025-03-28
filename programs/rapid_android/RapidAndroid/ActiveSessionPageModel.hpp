// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef PROGRAMS_RAPID_ANDROID_ACTIVESESSIONPAGEMODEL_HPP
#define PROGRAMS_RAPID_ANDROID_ACTIVESESSIONPAGEMODEL_HPP

#include <QObject>
#include <QQmlEngine>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <rest/qt/QRestClient.hpp>
#include <workflow/qt/RestActiveSession.hpp>

namespace Rapid::Android
{

/**
 * @brief Holds the state of the ActiveSessionPage QML context and implements the view logic.
 */
class ActiveSessionPageModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @property Rapid::Workflow::Qt::RestActiveSession
     *
     * Gives the REST active session for displaying the information for the active session
     */
    Q_PROPERTY(Rapid::Workflow::Qt::RestActiveSession const* activeSession READ getActiveSession CONSTANT)

    /**
     * @property Rapid::Common::Qt::DeviceSettings
     *
     * This property holds the laptimer settings that are used by the SessionPage
     */
    Q_PROPERTY(Rapid::Common::Qt::DeviceSettings activeLaptimer READ getActiveLaptimer WRITE setActiveLaptimer NOTIFY
                   activeLaptimerChanged REQUIRED)
public:
    Q_DISABLE_COPY_MOVE(ActiveSessionPageModel)

    /**
     * @brief Creates an instance of the @ref Rapid::Android::ActiveSessionPageModel
     */
    [[nodiscard]] ActiveSessionPageModel();

    /** @cond Doxygen_Suppress */
    ~ActiveSessionPageModel() noexcept override;
    /** @endcond */

Q_SIGNALS:

    /**
     * @brief This signal is emitted when the active laptimer is changed.
     *
     * @details The active laptimer is changed when in the laptimer page a different laptimer is activated.
     */
    void activeLaptimerChanged();

private:
    [[nodiscard]] Rapid::Workflow::Qt::RestActiveSession* getActiveSession() noexcept;
    [[nodiscard]] Rapid::Common::Qt::DeviceSettings getActiveLaptimer() const noexcept;
    void setActiveLaptimer(Rapid::Common::Qt::DeviceSettings activeLaptimer);

    Rapid::Rest::QRestClient mRestclient;
    Rapid::Workflow::Qt::RestActiveSession mRestActiveSession{&mRestclient};
    Rapid::Common::Qt::DeviceSettings mActiveLaptimer;
};

} // namespace Rapid::Android

#endif // !PROGRAMS_RAPID_ANDROID_ACTIVESESSIONPAGEMODEL_HPP
