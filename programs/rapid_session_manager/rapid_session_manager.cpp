// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QTimer>
#include <SessionManager.hpp>
#include <system/EventLoop.hpp>
#include <system/qt/RapidApplication.hpp>

int main(int argc, char** argv)
{
    auto app = Rapid::System::Qt::RapidApplication{argc, argv};
    app.setApplicationName("lappy");
    app.setOrganizationName("Lappy");
    app.setOrganizationDomain("de.lappy");

    auto sessionMgr = Rapid::SessionManager::SessionManager{};
    sessionMgr.show();

    return app.exec();
}
