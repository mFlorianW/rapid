// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionAnalyzer.hpp"
#include <QQuickWindow>

namespace Rapid::SessionAnalyzer
{

SessionAnalyzer::SessionAnalyzer()
{
    mEngine.load(QUrl{"qrc:/qt/qml/Rapid/SessionAnalyzer/qml/MainWindow.qml"});
}

void SessionAnalyzer::show() const noexcept
{
    auto* topLevel = mEngine.rootObjects().value(0);
    auto* window = qobject_cast<QQuickWindow*>(topLevel);
    window->show();
}

} // namespace Rapid::SessionAnalyzer
