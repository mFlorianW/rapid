// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <SessionAnalyzer.hpp>
#include <system/EventLoop.hpp>
#include <system/qt/RapidQApplication.hpp>

int main(int argc, char** argv)
{
    auto app = Rapid::System::Qt::RapidQApplication{argc, argv};
    app.setApplicationName("session_analyzer");
    app.setOrganizationName("rapid");
    app.setOrganizationDomain("de.rapid");

    auto sessionAnalyzer = Rapid::SessionAnalyzer::SessionAnalyzer{};
    sessionAnalyzer.show();

    return app.exec();
}
