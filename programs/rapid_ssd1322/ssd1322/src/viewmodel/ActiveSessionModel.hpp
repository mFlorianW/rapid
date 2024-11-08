// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef ACTIVESESSIONMODEL_HPP
#define ACTIVESESSIONMODEL_HPP

#include <IActiveSessionWorkflow.hpp>
#include <ITrackDatabase.hpp>
#include <ITrackDetectionWorkflow.hpp>
#include <kdbindings/property.h>

class ActiveSessionModel
{
public:
    ActiveSessionModel(Rapid::Workflow::ITrackDetectionWorkflow& trackDetector,
                       Rapid::Workflow::IActiveSessionWorkflow& activeWorkSessionFlow,
                       Rapid::Storage::ITrackDatabase& trackDatabase);

    /**
     * Gives the last completed lap time. If none is present 00:00:00.000 as string is returned
     * @return The last completed lap time.
     */
    std::string getLastLapTime() const noexcept;

    /**
     * Gives the last completed sector time. If none is present 00:00:00.000 as string is returned.
     * @return The last completed lap time.
     */
    std::string getLastSector() const noexcept;

    /**
     * Confirm the detected track. True means that the track is confirmed by the user
     * and active session will be started and decativates the track detection.
     * @param confirmed True track is confirmed by the user otherwise false.
     */
    void confirmTrackDetection(bool confirmed);

    /**
     * Gives the last detected track data.
     * @return The last detected track data.
     */
    Rapid::Common::TrackData getDetectedTrack() const noexcept;

    /**
     * This property holds the track data when a track is detected.
     */
    KDBindings::Signal<> trackDetected;

    /**
     * This signal is emitted
     */
    KDBindings::Signal<> lapFinished;

    /**
     * This signal is emitted when the sector is finished
     */
    KDBindings::Signal<> sectorFinished;

    /**
     * Holds the lapCount of the current session
     */
    KDBindings::Property<std::uint16_t> lapCount;

    /**
     * Holds the current laptime as string
     */
    KDBindings::Property<Rapid::Common::Timestamp> currentLaptime;

    /**
     * Holds the current sector time as string.
     */
    KDBindings::Property<Rapid::Common::Timestamp> currentSectorTime;

private:
    Rapid::Workflow::ITrackDetectionWorkflow& mTrackDetector;
    Rapid::Workflow::IActiveSessionWorkflow& mActiveSessionWorkFlow;
    Rapid::Storage::ITrackDatabase& mTrackDatabase;
    Rapid::Common::TrackData mTrackData;
};

#endif //! ACTIVESESSIONMODEL_HPP
