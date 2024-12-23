// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <EventLoop.hpp>
#include <RapidApplication.hpp>
#include <SessionAnalyzer.hpp>

int main(int argc, char** argv)
{
    auto app = Rapid::System::Qt::RapidApplication{argc, argv};
    app.setApplicationName("session_analyzer");
    app.setOrganizationName("rapid");
    app.setOrganizationDomain("de.rapid");

    auto sessionAnalyzer = Rapid::SessionAnalyzer::SessionAnalyzer{};
    sessionAnalyzer.show();

    return app.exec();
}
