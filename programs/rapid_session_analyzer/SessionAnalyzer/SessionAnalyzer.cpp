// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionAnalyzer.hpp"
#include "ui_SessionAnalyzer.h"
#include <QQuickWindow>

namespace Rapid::SessionAnalyzer
{

SessionAnalyzer::SessionAnalyzer()
    : QMainWindow{}
    , mMainWindow{std::make_unique<Ui::SessionAnalyzer>()}
{
    mMainWindow->setupUi(this);
    connect(mMainWindow->actionQuit, &QAction::triggered, this, []() {
        QApplication::exit();
    });
}

SessionAnalyzer::~SessionAnalyzer() = default;

} // namespace Rapid::SessionAnalyzer
