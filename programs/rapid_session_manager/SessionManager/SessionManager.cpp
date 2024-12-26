// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionManager.hpp"
#include "ui_SessionManager.h"

namespace Rapid::SessionManager
{

SessionManager::SessionManager()
    : mSessionManager{std::make_unique<Ui::SessionManager>()}
{
    mSessionManager->setupUi(this);

    connect(mSessionManager->actionQuit, &QAction::triggered, this, [] {
        QApplication::exit();
    });
}

SessionManager::~SessionManager() = default;

} // namespace Rapid::SessionManager
