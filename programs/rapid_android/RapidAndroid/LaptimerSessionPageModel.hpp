// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef PROGRAMS_RAPID_ANDROID_LAPTIMERSESSIONPAGEMODEL_HPP
#define PROGRAMS_RAPID_ANDROID_LAPTIMERSESSIONPAGEMODEL_HPP

#include <QAbstractItemModel>
#include <QObject>
#include <QQmlEngine>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <rest/qt/QRestClient.hpp>
#include <workflow/qt/RestSessionManagementWorkflow.hpp>

namespace Rapid::Android
{

/**
 * @brief Holds the state of the LaptimerSessionPageModel QML context and implements the view logic.
 */
class LaptimerSessionPageModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @property Rapid::Common::Qt::DeviceSettings
     *
     * This property holds the laptimer settings that are used by the SessionPage
     */
    Q_PROPERTY(Rapid::Common::Qt::DeviceSettings activeLaptimer READ getActiveLaptimer WRITE setActiveLaptimer NOTIFY
                   activeLaptimerChanged REQUIRED)

    /**
     * @property Rapid::Common::Qt::SessionMetaDataListModel
     *
     * This property holds the list model with the session data of the active laptimer.
     * Initially the model is empty to fill the model call @ref Rapid::Android::LaptimerSessionPageModel::updateSessionMetadata
     */
    Q_PROPERTY(
        Rapid::Common::Qt::SessionMetaDataListModel* sessionMetadataListModel READ getSessionMetadataListModel CONSTANT)
public:
    /** @cond Doxygen_Suppress */
    Q_DISABLE_COPY_MOVE(LaptimerSessionPageModel)
    LaptimerSessionPageModel();
    ~LaptimerSessionPageModel() override;
    /** @endcond */

    /**
     * @brief Fetches all @ref Rapid::Common::SessionMetaData from the active laptimer
     *
     * @details This function updates the content of session meta data model property.
     */
    Q_INVOKABLE void updateSessionMetadata() noexcept;

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the active laptimer is changed.
     *
     * @details The active laptimer is changed when in the laptimer page a different laptimer is activated.
     */
    void activeLaptimerChanged();

private:
    [[nodiscard]] Rapid::Common::Qt::DeviceSettings getActiveLaptimer() const noexcept;
    void setActiveLaptimer(Rapid::Common::Qt::DeviceSettings activeLaptimer) noexcept;
    [[nodiscard]] Rapid::Common::Qt::SessionMetaDataListModel* getSessionMetadataListModel() noexcept;

    Common::Qt::DeviceSettings mActiveLaptimer;
    Rapid::Rest::QRestClient mRestclient;
    std::unique_ptr<Rapid::Workflow::Qt::RestSessionManagementWorkflow> mRestSessionManagement;
};

} // namespace Rapid::Android

#endif
