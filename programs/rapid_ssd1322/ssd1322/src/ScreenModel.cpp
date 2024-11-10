// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "../include/ScreenModel.hpp"
#include "MainScreen.hpp"
#include "MainScreenModel.hpp"
#include "MenuScreen.hpp"
#include "MenuScreenModel.hpp"
#include <lvgl.h>

struct ScreenModelPrivate
{
    explicit ScreenModelPrivate(ScreenModel& screenModel,
                                Rapid::Positioning::IGpsInformationProvider& gpsInfoProvider,
                                Rapid::Positioning::IGpsPositionProvider& posDateTimeProvider,
                                Rapid::Storage::ISessionDatabase& sessionDatabase,
                                Rapid::Storage::ITrackDatabase& trackDatabase)
        : mMainScreen{gpsInfoProvider}
        , mMenuModel{screenModel, sessionDatabase}
        , mMainScreenModel{screenModel, posDateTimeProvider, sessionDatabase, trackDatabase}
    {
    }

    // Screens
    MainScreen mMainScreen;
    MenuScreen mMenuScreen;

    // Screen Models
    MenuScreenModel mMenuModel;
    MainScreenModel mMainScreenModel;
};

ScreenModel::ScreenModel(Rapid::Positioning::IGpsInformationProvider& gpsInfoProvider,
                         Rapid::Positioning::IGpsPositionProvider& posDateTimeProvider,
                         Rapid::Storage::ISessionDatabase& sessionDatabase,
                         Rapid::Storage::ITrackDatabase& trackDatabase)
    : mD{std::make_unique<ScreenModelPrivate>(*this,
                                              gpsInfoProvider,
                                              posDateTimeProvider,
                                              sessionDatabase,
                                              trackDatabase)}
{
    // Connect to menu model view changed
    mD->mMenuModel.viewChanged.connect(&ScreenModel::activateMenuScreen, this);
    mD->mMenuModel.closeMenu.connect(&ScreenModel::activateMainScreen, this);
}

ScreenModel::~ScreenModel() = default;

void ScreenModel::activateMainScreen()
{
    mD->mMenuScreen.setVisible(false);
    auto& mainScreenView = mD->mMainScreenModel.getActiveView();
    mD->mMainScreen.setScreenContent(&mainScreenView);
    lv_scr_load(mD->mMainScreen.getScreen());
    mD->mMainScreen.setVisible(true);
}

void ScreenModel::activateMenuScreen()
{
    mD->mMainScreen.setVisible(false);
    auto& menuView = mD->mMenuModel.getActiveView();
    mD->mMenuScreen.setScreenContent(&menuView);
    lv_scr_load(mD->mMenuScreen.getScreen());
    mD->mMenuScreen.setVisible(true);
}
