// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidQApplication.hpp"
#include "EventLoopIntegration.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::System::Qt
{

RapidQApplication::RapidQApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    if (not EventLoopIntegration::makeEventLoopIntegration()) {
        SPDLOG_CRITICAL("Faild to setup rapid eventloop integration.");
    }
}

RapidQApplication::~RapidQApplication() = default;

} // namespace Rapid::System::Qt
