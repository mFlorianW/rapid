// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestGpsSource.hpp"
#include <QApplication>
#include <system/qt/RapidQApplication.hpp>

int main(int argc, char* argv[])
{
    auto app = Rapid::System::Qt::RapidQApplication(argc, argv);
    app.setApplicationName("lappy");
    app.setOrganizationName("Lappy");
    app.setOrganizationDomain("de.lappy");

    auto restgpssource = RestGpsSource{};
    restgpssource.show();

    return app.exec();
}
