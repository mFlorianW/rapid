// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Screen.hpp"
#include <cstdio>
#include <lv_obj.h>
#include <spdlog/spdlog.h>

Screen::Screen()
    : m_screen{lv_obj_create(nullptr, nullptr)}
    , mPopupCloseCommand{*this}
    , mPopupConfirmCommand{*this}
{
    // setup screen
    lv_style_init(&mScreenStyle);
    lv_obj_set_size(m_screen, 256, 64);
    lv_style_set_bg_color(&mScreenStyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(m_screen, LV_OBJ_PART_MAIN, &mScreenStyle);

    // setup event handling
    lv_obj_set_event_cb(m_screen, Screen::handleLvglEvent);
    lv_obj_set_user_data(m_screen, this);

    mPopupView.setOpenCommand(&mPopupConfirmCommand);
    mPopupView.setCloseCommand(&mPopupCloseCommand);
}

Screen::~Screen()
{
    lv_obj_del(m_screen);
}

lv_obj_t* Screen::getScreen() const
{
    return m_screen;
}

void Screen::open()
{
    if (mPopupRequest == nullptr) {
        return;
    }

    closePopup(PopupReturnType::Confirmed);
}

void Screen::close()
{
    if (mPopupRequest == nullptr) {
        return;
    }

    closePopup(PopupReturnType::Cancled);
}

void Screen::setScreenContent(View* content)
{
    // Restore old parent otherwise the Screen may contain
    // old objects.
    restoreParent();

    mOldScreenContentParent = lv_obj_get_parent(content->get_screen_content());
    lv_obj_set_parent(content->get_screen_content(), m_screen);
    lv_event_send_refresh_recursive(m_screen);
    mActiveView = content;
    mActiveView->setVisible(true);
    mPopupRequestConnectionHandle = mActiveView->requestPopup.connect(&Screen::onPopupRequested, this);
}

bool Screen::isVisible() const noexcept
{
    return mVisible;
}

void Screen::handleLvglEvent(lv_obj_t* obj, lv_event_t event)
{
    if (event != LV_EVENT_KEY) {
        return;
    }

    auto* screen = static_cast<Screen*>(lv_obj_get_user_data(obj));
    auto* view = screen->mPopupActive ? &screen->mPopupView : screen->mActiveView;
    auto const* keyType = static_cast<lv_key_t const*>(lv_event_get_data());
    if (view && *keyType == LV_KEY_ENTER) {
        view->handleEnter();
    } else if (view && *keyType == LV_KEY_ESC) {
        view->handleEscape();
    } else if (view && *keyType == LV_KEY_UP) {
        view->handleButtonUp();
    } else if (view && *keyType == LV_KEY_DOWN) {
        view->handleButtonDown();
    } else {
        printf("Unsupported key event. key type: %d", *keyType);
    }
}

void Screen::closePopup(PopupReturnType popupReturnType)
{
    mPopupActive = false;
    mPopupView.setMainText("");
    mPopupView.setSecondaryText("");
    if (mPopupView.getType() == Type::Confirmattion) {
        mPopupRequest->confirmed.emit(popupReturnType);
    }
    mPopupView.setVisible(false);
    lv_obj_set_parent(mPopupView.get_screen_content(), mOldPopupParent);
}

void Screen::onPopupRequested(PopupRequest const& popupRequest)
{
    mPopupActive = true;
    mPopupRequest = &popupRequest;
    mOldPopupParent = lv_obj_get_parent(mPopupView.get_screen_content());
    mPopupView.setMainText(mPopupRequest->getMainText());
    mPopupView.setSecondaryText(mPopupRequest->getSecondaryText());
    mPopupView.setType(mPopupRequest->getPopupType());
    if (mPopupView.getType() == Type::NoConfirmation && mPopupRequest->isAutoClosing()) {
        mPopupView.setAutoClosingTimeout(popupRequest.getAutoClosingTimeout());
    }
    mPopupView.setVisible(true);
    lv_obj_set_parent(mPopupView.get_screen_content(), lv_layer_top());
}

void Screen::restoreParent()
{
    if (mActiveView != nullptr) {
        if (mOldScreenContentParent != nullptr) {
            lv_obj_set_parent(mActiveView->get_screen_content(), mOldScreenContentParent);
        }
        mActiveView->setVisible(false);

        try {
            mActiveView->requestPopup.disconnect(mPopupRequestConnectionHandle);
        } catch (std::out_of_range const& e) {
            spdlog::error("Error on connection disconnect. Error: {}", e.what());
        }
    }
}

void Screen::setVisible(bool const visible) noexcept
{
    mVisible = visible;
    if (!mVisible) {
        restoreParent();
    }
}
