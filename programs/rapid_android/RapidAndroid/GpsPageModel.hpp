// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_PROGRAMS_RAPID_ANDROID_GPSPAGEMODEL_HPP
#define RAPID_PROGRAMS_RAPID_ANDROID_GPSPAGEMODEL_HPP

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <common/qt/GlobalSettingsTypes.hpp>
#include <rest/qt/QRestClient.hpp>
#include <workflow/GpsRestResourceReader.hpp>

namespace Rapid::Android
{
/**
 * @brief Holds the state of the GpsPage QML context and implements the view logic.
 */
class GpsPageModel : public QObject
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
     * @property QString
     *
     * This poperty holds the state of the used amount of GPS satellites
     */
    Q_PROPERTY(QString satellites READ getSatellites NOTIFY satellitesChanged)

    /**
     * @property QString
     *
     * This property holdes the source string for the GPS status icon
     */
    Q_PROPERTY(QString gpsStatus READ getGpsStatus NOTIFY gpsStatusChanged)

    /**
     * @property bool
     *
     * True periodic GPS information upates are active
     * False periodic GPS information updates are not active
     */
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningChanged)

    /**
     * @property QString
     *
     * This property holds the action icon source state of the GPSPage.
     */
    Q_PROPERTY(QString activeIcon READ getActiveIcon NOTIFY activeIconChanged);

public:
    /** @cond Doxygen_Suppress */
    Q_DISABLE_COPY_MOVE(GpsPageModel)
    GpsPageModel();
    ~GpsPageModel() override;
    /** @endcond */

    /**
     * @brief Starts the periodic update of the GPS informations
     */
    Q_INVOKABLE void start();

    /**
     * @brief Stops the periodic update of the GPS informations
     */
    Q_INVOKABLE void stop();

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the active laptimer is changed.
     *
     * @details The active laptimer is changed when in the laptimer page a different laptimer is activated.
     */
    void activeLaptimerChanged();

    /**
     * @brief This signal is emitted when the amount of used GPS satellites is changed.
     *
     * @details The amount only changes when the periodic GPS information updates are active.
     */
    void satellitesChanged();

    /**
     * @brief This signal is emitted when the GPS status is changed.
     *
     * @details The GPS status only changes when the periodic GPS information updates are active.
     */
    void gpsStatusChanged();

    /**
     * @brief This signal is emitted when the running is changed.
     *
     * @details The running state is changed when the periodic GPS information updates are started or stopped.
     */
    void runningChanged();

    /**
     * @brief This signal is emitted when the active icon is changed.
     *
     * @details The active icon is changed when the periodic GPS information updates are started or stopped.
     */
    void activeIconChanged();

private:
    [[nodiscard]] Rapid::Common::Qt::DeviceSettings getActiveLaptimer() const noexcept;
    void setActiveLaptimer(Rapid::Common::Qt::DeviceSettings activeLaptimer) noexcept;
    QString getGpsStatus() const noexcept;
    QString getSatellites() const noexcept;
    bool isRunning() const noexcept;
    QString getActiveIcon() const noexcept;
    void fetchFixModeResult();
    void fetchSatellites();

    Common::Qt::DeviceSettings mActiveLaptimer;
    QTimer mUpdateTimer;
    Workflow::GpsRestResourceReader mGpsRestResourceReader;
    std::shared_ptr<Workflow::FixModeResult> mFixModeResult;
    KDBindings::ScopedConnection mFixModeResultConnection;
    std::shared_ptr<Workflow::SatellitesResult> mSatellitesResult;
    KDBindings::ScopedConnection mSatellitesResultConnection;
    Rest::QRestClient mRestClient;
    Positioning::GpsFixMode mFixMode{Positioning::GpsFixMode::NoFix};
    std::uint8_t mSatellites{0};
};

} // namespace Rapid::Android

#endif // !RAPID_PROGRAMS_RAPID_ANDROID_GPSPAGEMODEL_HPP
