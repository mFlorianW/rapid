// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <IGPSInformationProvider.hpp>
#include <IGpsPositionProvider.hpp>
#include <ISessionDatabase.hpp>
#include <ITrackDatabase.hpp>
#include <memory>

/**
 * ScreenModel setups the Shell
 */
struct ScreenModelPrivate;
class ScreenModel
{
public:
    /**
     * Creates an Instance of the ScreenModel
     * @param gpsInfoProvider The GPS information provider service.
     * @param posInfoProvider  The GPS position information provider service.
     */
    ScreenModel(Rapid::Positioning::IGpsInformationProvider& gpsInfoProvider,
                Rapid::Positioning::IGpsPositionProvider& posDateTimeProvider,
                Rapid::Storage::ISessionDatabase& sessionDatabase,
                Rapid::Storage::ITrackDatabase& trackDatabase);

    virtual ~ScreenModel();
    ScreenModel& operator=(ScreenModel const&) = delete;
    ScreenModel(ScreenModel const&) = delete;
    ScreenModel& operator=(ScreenModel&&) noexcept = delete;
    ScreenModel(ScreenModel const&&) noexcept = delete;

    /**
     * Activates the mainScreen view
     */
    void activateMainScreen();

    /**
     * Activates the menu screen
     */
    void activateMenuScreen();

private:
    std::unique_ptr<ScreenModelPrivate> mD;
};
