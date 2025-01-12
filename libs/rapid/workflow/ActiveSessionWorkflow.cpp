// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ActiveSessionWorkflow.hpp"
#include <spdlog/spdlog.h>

using namespace Rapid::Common;

namespace Rapid::Workflow
{

ActiveSessionWorkflow::ActiveSessionWorkflow(Positioning::IGpsPositionProvider& positionDateTimeProvider,
                                             Algorithm::ILaptimer& laptimer,
                                             Storage::ISessionDatabase& database)
    : mDateTimeProvider{positionDateTimeProvider}
    , mLaptimer{laptimer}
    , mDatabase{database}
{
}

void ActiveSessionWorkflow::startActiveSession() noexcept
{
    try {
        std::ignore = mLaptimer.lapFinished.connect(&ActiveSessionWorkflow::onLapFinished, this);
        std::ignore = mLaptimer.sectorFinished.connect(&ActiveSessionWorkflow::onSectorFinished, this);
        std::ignore =
            mLaptimer.currentLaptime.valueChanged().connect(&ActiveSessionWorkflow::onCurrentLaptimeChanged, this);
        std::ignore =
            mLaptimer.currentSectorTime.valueChanged().connect(&ActiveSessionWorkflow::onCurrentSectorTimeChanged,
                                                               this);
        mLaptimer.setTrack(mTrack);
        std::ignore = mLaptimer.lapStarted.connect([this]() {
            mLapActive = true;
        });

        mPositionDateTimeUpdateHandle = mDateTimeProvider.gpsPosition.valueChanged().connect([this]() {
            mLaptimer.updatePositionAndTime(mDateTimeProvider.gpsPosition.get());
            if (mLapActive) {
                mCurrentLap.addPosition(mDateTimeProvider.gpsPosition.get());
            }
        });
        auto dateTime = mDateTimeProvider.gpsPosition.get();
        mSession = Common::SessionData{mTrack, dateTime.getDate(), dateTime.getTime()};
        lapCount.set(0);
    } catch (std::exception const& e) {
        spdlog::error("Unknow Error on starting active session. Error: {}", e.what());
    }
}

void ActiveSessionWorkflow::stopActiveSession() noexcept
{
    try {
        mDateTimeProvider.gpsPosition.valueChanged().disconnect(mPositionDateTimeUpdateHandle);
        mSession = std::nullopt;
    } catch (std::exception const& e) {
        spdlog::error("Unknow Error on stopping active session. Error: {}", e.what());
    }
}

void ActiveSessionWorkflow::setTrack(Common::TrackData const& track) noexcept
{
    mTrack = track;
}

std::optional<Common::SessionData> ActiveSessionWorkflow::getSession() const noexcept
{
    return mSession;
}

void Rapid::Workflow::ActiveSessionWorkflow::addSectorTime()
{
    auto const sectorTime = mLaptimer.getLastSectorTime();
    lastSectorTime.set(sectorTime);
    mCurrentLap.addSectorTime(sectorTime);
}

void ActiveSessionWorkflow::onLapFinished()
{
    // Session not started immediately return.
    if (!mSession) {
        return;
    }

    addSectorTime();

    mSession->addLap(mCurrentLap);
    mDatabase.storeSession(mSession.value());
    lastLaptime.set(mCurrentLap.getLaptime());
    mCurrentLap = Common::LapData{};

    auto const newLapCount = lapCount.get() + 1;
    lapCount.set(newLapCount);
    lapFinished.emit();
}

void ActiveSessionWorkflow::onSectorFinished()
{
    addSectorTime();
    sectorFinshed.emit();
}

void ActiveSessionWorkflow::onCurrentLaptimeChanged()
{
    currentLaptime.set(mLaptimer.currentLaptime.get());
}

void ActiveSessionWorkflow::onCurrentSectorTimeChanged()
{
    currentSectorTime.set(mLaptimer.currentSectorTime.get());
}

} // namespace Rapid::Workflow
