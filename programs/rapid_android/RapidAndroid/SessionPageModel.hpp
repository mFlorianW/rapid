// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef PROGRAMS_RAPID_ANDROID_SESSIONPAGEMODEL_HPP
#define PROGRAMS_RAPID_ANDROID_SESSIONPAGEMODEL_HPP

#include "Database.hpp"
#include "workflow/qt/SessionAnalyzeWorkflow.hpp"
#include <QObject>
#include <QQmlEngine>
#include <common/qt/SessionMetaDataListModel.hpp>
#include <storage/SqliteSessionDatabase.hpp>
#include <workflow/qt/LocalSessionManagement.hpp>

namespace Rapid::Android
{

class SessionPageModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Rapid::Common::Qt::SessionMetaDataListModel* sessionListModel READ getSessionListModel CONSTANT)

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
     * @brief Gives the model for displaying the session in a list.
     *
     * @return A list model with the stored sessions.
     */
    [[nodiscard]] Rapid::Common::Qt::SessionMetaDataListModel* getSessionListModel() const noexcept;

    /**
     * @brief Gives the model for displaying the session in a list.
     *
     * @return A list model with the stored sessions.
     */
    [[nodiscard]] Rapid::Common::Qt::LapListModel* getLapListModel() const noexcept;

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

private:
    void handleDbQueryResult();

    std::unique_ptr<Storage::ISessionDatabase> mSessionDb;
    Rapid::Workflow::Qt::LocalSessionManagement mLocalSessionMgmt;
    Rapid::Workflow::Qt::SessionAnalyzeWorkflow mSessionAnalyzeWorkflow;
    KDBindings::ScopedConnection mLapListModelChangedConnection;
    std::shared_ptr<Storage::GetSessionResult> mDbQuery;
    KDBindings::ScopedConnection mDbQueryDoneConnection;
};

} // namespace Rapid::Android

#endif //! PROGRAMS_RAPID_ANDROID_SESSIONPAGEMODEL_HPP
