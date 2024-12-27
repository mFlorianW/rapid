// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "CliOptions.hpp"
#include "SessionDownloader.hpp"
#include <QTimer>
#include <RapidApplication.hpp>
#include <system/EventLoop.hpp>

int main(int argc, char** argv)
{
    auto app = Rapid::System::Qt::RapidApplication{argc, argv};
    app.setApplicationName("lappy");
    app.setOrganizationName("Lappy");
    app.setOrganizationDomain("de.lappy");

    auto const options = Rapid::SessionDl::CliOptions{argc, argv};
    auto const sDl = Rapid::SessionDl::SessionDownloader{options.getHostAddress(), options.getPort()};
    sDl.show();

    return app.exec();
}
