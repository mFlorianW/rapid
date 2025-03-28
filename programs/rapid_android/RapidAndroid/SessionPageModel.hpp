// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef PROGRAMS_RAPID_ANDROID_SESSIONPAGEMODEL_HPP
#define PROGRAMS_RAPID_ANDROID_SESSIONPAGEMODEL_HPP

#include "Database.hpp"
#include <QObject>
#include <QQmlEngine>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <common/qt/SessionMetaDataListModel.hpp>
#include <common/qt/SessionMetaDataSortListModel.hpp>
#include <rest/qt/QRestClient.hpp>
#include <storage/SqliteSessionDatabase.hpp>
#include <workflow/ActiveSessionWorkflow.hpp>
#include <workflow/qt/LocalSessionManagement.hpp>
#include <workflow/qt/RestActiveSession.hpp>
#include <workflow/qt/SessionAnalyzeWorkflow.hpp>

namespace Rapid::Android
{

/**
 * @brief Holds the state of the SessionPage QML context and implements the view logic.
 */
class SessionPageModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * @property Rapid::Common::Qt::LapListModel*
     *
     * Gives a session list model for displaying the local stored sessions
     */
    Q_PROPERTY(Rapid::Common::Qt::SessionMetaDataSortListModel* sessionListModel READ getSessionListModel CONSTANT)

    /**
     * @property Rapid::Common::Qt::LapListModel*
     *
     * Gives the a list model for display the laps of a session
     */
    Q_PROPERTY(Rapid::Common::Qt::LapListModel* lapListModel READ getLapListModel NOTIFY lapListModelChanged)

public:
    Q_DISABLE_COPY_MOVE(SessionPageModel)

    /**
     * @brief Creates an instance of the @ref Rapid::Android::SessionPageModel
     *
     * @param sessionDb The session database that is used for the view
     */
    [[nodiscard]] SessionPageModel(
        std::unique_ptr<Storage::ISessionDatabase> sessionDb =
            std::make_unique<Storage::SqliteSessionDatabase>(getDatabaseLocation().toStdString().c_str()));

    /** @cond Doxygen_Suppress */
    ~SessionPageModel() override;
    /** @endcond */

    /**
     * @brief Analyze session under the given index
     *
     * @param sessionIndex The index of the session that shall be analyzed
     */
    Q_INVOKABLE void analyzeSession(quint32 sessionIndex);

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the lap list model is changed
     *
     * @details The lap list model typically changes when a new session for the analyization is set.
     */
    void lapListModelChanged();

    /**
     * @brief This signal is emitted when the active laptimer is changed.
     *
     * @details The active laptimer is changed when in the laptimer page a different laptimer is activated.
     */
    void activeLaptimerChanged();

private:
    [[nodiscard]] Rapid::Common::Qt::SessionMetaDataSortListModel* getSessionListModel() noexcept;
    [[nodiscard]] Rapid::Common::Qt::LapListModel* getLapListModel() noexcept;

    void handleDbQueryResult();

    std::unique_ptr<Storage::ISessionDatabase> mSessionDb;
    Rapid::Workflow::Qt::LocalSessionManagement mLocalSessionMgmt;
    Rapid::Workflow::Qt::SessionAnalyzeWorkflow mSessionAnalyzeWorkflow;
    KDBindings::ScopedConnection mLapListModelChangedConnection;
    std::shared_ptr<Storage::GetSessionResult> mDbQuery;
    KDBindings::ScopedConnection mDbQueryDoneConnection;
    Rapid::Common::Qt::SessionMetaDataSortListModel mSessionMetaSortModel;
};

} // namespace Rapid::Android

#endif //! PROGRAMS_RAPID_ANDROID_SESSIONPAGEMODEL_HPP
