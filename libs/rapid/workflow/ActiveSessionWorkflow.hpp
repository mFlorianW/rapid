// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef ACTIVESESSIONWORKFLOW_HPP
#define ACTIVESESSIONWORKFLOW_HPP

#include "IActiveSessionWorkflow.hpp"
#include <algorithm/ILaptimer.hpp>
#include <positioning/IGpsPositionProvider.hpp>
#include <storage/ISessionDatabase.hpp>

namespace Rapid::Workflow
{
class ActiveSessionWorkflow : public IActiveSessionWorkflow
{
public:
    /**
     * Creates an instance of the ActiveSessionWorkflow
     * @param timeDateProvider The date and time information provider to get the latest time date informations.
     * @param laptimer The laptimer that is used to get notified about new laps and sectors.
     * @param database The session database that shall be used to store the data.
     */
    ActiveSessionWorkflow(Positioning::IGpsPositionProvider& positionDateTimeProvider,
                          Algorithm::ILaptimer& laptimer,
                          Storage::ISessionDatabase& database);

    /**
     * @copydoc IActiveSessionWorkflow::startActiveSession()
     */
    void startActiveSession() noexcept override;

    /**
     * @copydoc IActiveSessionWorkflow::stopActiveSession()
     */
    void stopActiveSession() noexcept override;

    /**
     * @copydoc IActiveSessionWorkflow::setTrack()
     */
    void setTrack(Common::TrackData const& track) noexcept override;

    /**
     * @copydoc IActiveSessionWorkflow::getActiveSession()
     */
    std::optional<Common::SessionData> getSession() const noexcept override;

private:
    /**
     * This function is called when the lap timer emits the lapFinished.
     */
    void onLapFinished();

    /**
     * This slot is called when the lap timer emit the sectorFinished.
     */
    void onSectorFinished();

    /**
     * This slot is called when the lap timer property currentLaptime changes.
     */
    void onCurrentLaptimeChanged();

    /**
     * This slot is caled when the lap timer property currentSector changes.
     */
    void onCurrentSectorTimeChanged();

private:
    void addSectorTime();

    Positioning::IGpsPositionProvider& mDateTimeProvider;
    Algorithm::ILaptimer& mLaptimer;
    Storage::ISessionDatabase& mDatabase;
    std::optional<Common::SessionData> mSession;
    Common::TrackData mTrack;
    Common::LapData mCurrentLap;
    bool mLapActive = false;

    KDBindings::ConnectionHandle mPositionDateTimeUpdateHandle;
};

} // namespace Rapid::Workflow

#endif // ACTIVESESSIONWORKFLOW_HPP
