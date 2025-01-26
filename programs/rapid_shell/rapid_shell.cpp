// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidShell.hpp"
#include <common/qt/GlobalSettingsReader.hpp>
#include <system/qt/RapidQApplication.hpp>

int main(int argc, char* argv[])
{
    auto app = Rapid::System::Qt::RapidQApplication{argc, argv};
    app.setApplicationName("rapid");
    app.setOrganizationName("Rapid");
    app.setOrganizationDomain("de.rapid");

    auto rapidShell = Rapid::RapidShell::RapidShell{};
    rapidShell.show();

    app.exec();
}
