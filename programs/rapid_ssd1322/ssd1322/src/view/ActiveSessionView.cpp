// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ActiveSessionView.hpp"

ActiveSessionView::ActiveSessionView(ActiveSessionModel& model)
    : MainScreenView{}
    , mActiveSessionModel{model}
{
    // Setup laptime view
    lv_style_init(&mLaptimeContainerStyle);
    lv_style_set_bg_color(&mLaptimeContainerStyle, LV_CONT_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_border_width(&mLaptimeContainerStyle, LV_CONT_PART_MAIN, 0);

    mLaptimeContainer = lv_cont_create(mScreenContent, nullptr);
    lv_obj_set_size(mLaptimeContainer, 256, 28);
    lv_obj_add_style(mLaptimeContainer, LV_CONT_PART_MAIN, &mLaptimeContainerStyle);

    auto xcoord = static_cast<std::int16_t>(lv_obj_get_width(mScreenContent) / static_cast<std::uint16_t>(2) -
                                            (lv_obj_get_width(mLaptimeContainer) / static_cast<std::uint16_t>(2)));
    lv_obj_set_x(mLaptimeContainer, xcoord);
    lv_obj_set_y(mLaptimeContainer, 4);
    lv_cont_set_fit(mLaptimeContainer, LV_FIT_NONE);
    lv_cont_set_layout(mLaptimeContainer, LV_LAYOUT_CENTER);

    lv_style_init(&mLaptimeLabelStyle);
    lv_style_set_text_font(&mLaptimeLabelStyle, LV_STATE_DEFAULT, &lv_font_montserrat_28);

    mLaptimeLabel = lv_label_create(mLaptimeContainer, nullptr);
    lv_obj_add_style(mLaptimeLabel, LV_LABEL_PART_MAIN, &mLaptimeLabelStyle);
    lv_obj_set_size(mLaptimeLabel, 256, 28);
    lv_label_set_text(mLaptimeLabel, "00:00.000");

    // Setup sector time view
    mSectorTimeContainer = lv_cont_create(mScreenContent, nullptr);
    lv_obj_set_size(mSectorTimeContainer, 256, 16);
    lv_obj_add_style(mSectorTimeContainer, LV_CONT_PART_MAIN, &mLaptimeContainerStyle);
    lv_obj_set_y(mSectorTimeContainer, 32);

    lv_style_init(&mSectorTimeLabelStyle);
    lv_style_set_text_font(&mSectorTimeLabelStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);

    mSectorTimeLabel = lv_label_create(mSectorTimeContainer, nullptr);
    lv_obj_add_style(mSectorTimeLabel, LV_LABEL_PART_MAIN, &mSectorTimeLabelStyle);
    lv_obj_set_size(mSectorTimeLabel, 256, 28);
    lv_label_set_text(mSectorTimeLabel, "00:00.000");

    // Setup lap count view
    lv_style_init(&mSectorTimeLabelStyle);
    lv_style_set_text_font(&mSectorTimeLabelStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);

    mLapCountLabel = lv_label_create(mScreenContent, nullptr);
    lv_obj_add_style(mLapCountLabel, LV_LABEL_PART_MAIN, &mSectorTimeLabelStyle);
    lv_obj_set_size(mLapCountLabel, 50, 16);
    lv_obj_align(mLapCountLabel, mScreenContent, LV_ALIGN_IN_BOTTOM_RIGHT, -38, 0);
    lv_label_set_text_fmt(mLapCountLabel, "Lap: %03d", 0);

    mPopupRequest.setPopupType(Type::Confirmattion);
    mPopupRequest.setMainText("Use track?");
    mPopupRequest.confirmed.connect(&ActiveSessionView::onTrackDetectionPopupClosed, this);
    mActiveSessionModel.trackDetected.connect(&ActiveSessionView::onTrackDetected, this);

    mLaptimePopupRequest.setPopupType(Type::NoConfirmation);
    mLaptimePopupRequest.setAutoClosing(true);
    mLaptimePopupRequest.setAutoClosingTimeout(std::chrono::seconds(5));

    mActiveSessionModel.currentLaptime.valueChanged().connect([this] {
        auto const timeStamp = mActiveSessionModel.currentLaptime.get();
        lv_label_set_text_fmt(mLaptimeLabel,
                              "%02d:%02d.%03d",
                              timeStamp.getMinute(),
                              timeStamp.getSecond(),
                              timeStamp.getFractionalOfSecond());
    });

    mActiveSessionModel.currentSectorTime.valueChanged().connect([this]() {
        auto timeStamp = mActiveSessionModel.currentSectorTime.get();
        lv_label_set_text_fmt(mSectorTimeLabel,
                              "%02d:%02d.%03d",
                              timeStamp.getMinute(),
                              timeStamp.getSecond(),
                              timeStamp.getFractionalOfSecond());
    });

    mActiveSessionModel.lapCount.valueChanged().connect([this]() {
        lv_label_set_text_fmt(mLapCountLabel, "Lap: %03d", mActiveSessionModel.lapCount.get());
    });

    mActiveSessionModel.lapFinished.connect(&ActiveSessionView::onLapFinished, this);
    mActiveSessionModel.sectorFinished.connect(&ActiveSessionView::onSectorFinshed, this);
}

ActiveSessionView::~ActiveSessionView()
{
    lv_obj_del(mLaptimeContainer);
    lv_obj_del(mLaptimeLabel);
    lv_obj_del(mSectorTimeContainer);
    lv_obj_del(mSectorTimeLabel);
    lv_obj_del(mLapCountLabel);
}

void ActiveSessionView::onTrackDetected()
{
    mPopupRequest.setSecondaryText("<" + mActiveSessionModel.getDetectedTrack().getTrackName() + ">");
    requestPopup.emit(mPopupRequest);
}

void ActiveSessionView::onLapFinished()
{
    mLaptimePopupRequest.setMainText(mActiveSessionModel.getLastLapTime());
    mLaptimePopupRequest.setSecondaryText("<LapFinished>");
    requestPopup.emit(mLaptimePopupRequest);
}

void ActiveSessionView::onSectorFinshed()
{
    mLaptimePopupRequest.setMainText(mActiveSessionModel.getLastSector());
    mLaptimePopupRequest.setSecondaryText("<Sector Finished>");
    requestPopup.emit(mLaptimePopupRequest);
}

void ActiveSessionView::onTrackDetectionPopupClosed(PopupReturnType returnType)
{
    auto confirmed = returnType == PopupReturnType::Confirmed;
    mActiveSessionModel.confirmTrackDetection(confirmed);
}
