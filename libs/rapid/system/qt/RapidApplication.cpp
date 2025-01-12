// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidApplication.hpp"
#include "EventLoopIntegration.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::System::Qt
{

RapidCoreApplication::RapidCoreApplication(int& argc, char** argv)
    : QCoreApplication(argc, argv)
{
    if (not EventLoopIntegration::makeEventLoopIntegration()) {
        SPDLOG_CRITICAL("Faild to setup rapid eventloop integration.");
    }
}

RapidCoreApplication::~RapidCoreApplication() = default;

RapidGuiApplication::RapidGuiApplication(int& argc, char** argv)
    : QGuiApplication(argc, argv)
{
    if (not EventLoopIntegration::makeEventLoopIntegration()) {
        SPDLOG_CRITICAL("Faild to setup rapid eventloop integration.");
    }
}

RapidGuiApplication::~RapidGuiApplication() = default;

RapidApplication::RapidApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    if (not EventLoopIntegration::makeEventLoopIntegration()) {
        SPDLOG_CRITICAL("Faild to setup rapid eventloop integration.");
    }
}

RapidApplication::~RapidApplication() = default;

} // namespace Rapid::System::Qt
