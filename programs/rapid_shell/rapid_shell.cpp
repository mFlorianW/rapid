// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidShell.hpp"
#include <RapidApplication.hpp>
#include <common/qt/GlobalSettingsReader.hpp>

int main(int argc, char* argv[])
{
    auto app = Rapid::System::Qt::RapidApplication{argc, argv};
    app.setApplicationName("rapid");
    app.setOrganizationName("Rapid");
    app.setOrganizationDomain("de.rapid");

    auto rapidShell = Rapid::RapidShell::RapidShell{};
    rapidShell.show();

    app.exec();
}
