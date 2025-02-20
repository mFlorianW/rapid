// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_QT_RESTACTIVESESSION_HPP
#define RAPID_WORKFLOW_QT_RESTACTIVESESSION_HPP

#include <QObject>
#include <QVariant>
#include <workflow/RestActiveSession.hpp>

namespace Rapid::Workflow::Qt
{

/**
 * Extends @ref Rapid::Workflow::RestActiveSession class for the Qt usage.
 */
class RestActiveSession : public QObject, public Workflow::RestActiveSession
{
    Q_OBJECT

    /**
     * @property QString
     *
     * @brief Gives the track name that is currently used in the active session.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateTrackData is called.
     */
    Q_PROPERTY(QString trackName READ getTrackName NOTIFY trackNameChanged)

    /**
     * @property quint64
     *
     * @brief Gives the lap count of the active session.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateLapData is called.
     */
    Q_PROPERTY(quint64 lapCount READ getLapCount NOTIFY lapCountChanged);

    /**
     * @property QString
     *
     * @brief Gives the current lap time.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateLapData is called.
     */
    Q_PROPERTY(QString currentLapTime READ getCurrentLapTime NOTIFY currentLapTimeChanged);

    /**
     * @property QString
     *
     * @brief Gives the current sector time.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateLapData is called.
     */
    Q_PROPERTY(QString currentSectorTime READ getCurrentSectorTime NOTIFY currentSectorTimeChanged);

public:
    Q_DISABLE_COPY_MOVE(RestActiveSession)

    /**
     * @copydoc Rapid::Workflow::RestActiveSession
     */
    RestActiveSession(Rest::IRestClient* restClient);

    /**
     * Default destructor
     */
    ~RestActiveSession() override;

    /**
     * @brief Gives the track name that is currently used in the active session.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateTrackData is called.
     */
    QString getTrackName() const noexcept;

    /**
     * @brief Gives the lap count of the active session.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateLapData is called.
     */
    quint64 getLapCount() const noexcept;

    /**
     * @brief Gives the current lap time.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateLapData is called.
     */
    QString getCurrentLapTime() const noexcept;

    /**
     * @brief Gives the current sector time.
     *
     * @details The property is updated when the function @ref Rapid::Workflow::RestActiveSession::updateLapData is called.
     */
    QString getCurrentSectorTime() const noexcept;

    /**
     * @copydoc Rapid::Workflow::IRestActiveSession::updateTrackData
     */
    Q_INVOKABLE void updateTrackData() noexcept override;

    /**
     * @copydoc Rapid::Workflow::IRestActiveSession::updateLapData
     */
    Q_INVOKABLE void updateLapData() noexcept override;

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the track name is changed.
     *
     * @details The change occurs when the active session data is updated with Rapid::Workflow::RestActiveSession::updateTrackData
     */
    void trackNameChanged();

    /**
     * @brief This signal is emitted when the lap count is changed.
     *
     * @details The change occurs when the active session data is updated with Rapid::Workflow::RestActiveSession::updateLapData
     */
    void lapCountChanged();

    /**
     * @brief This signal is emitted when the current lap time is changed.
     *
     * @details The change occurs when the active session data is updated with Rapid::Workflow::RestActiveSession::updateLapData
     */
    void currentLapTimeChanged();

    /**
     * @brief This signal is emitted when the current sector time is changed.
     *
     * @details The change occurs when the active session data is updated with Rapid::Workflow::RestActiveSession::updateLapData
     */
    void currentSectorTimeChanged();

private:
    KDBindings::ScopedConnection mTrackChangedConnection;
    KDBindings::ScopedConnection mLapCountConnection;
    KDBindings::ScopedConnection mCurrentLapTimeConnection;
    KDBindings::ScopedConnection mCurrentSectorTimeConnection;
};

} // namespace Rapid::Workflow::Qt

#endif // RAPID_WORKFLOW_QT_RESTACTIVESESSION_HPP
