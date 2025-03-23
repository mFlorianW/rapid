// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Logger.hpp"

namespace Rapid::System::Qt::Logger
{

void setDefaultLogger(std::shared_ptr<spdlog::logger> const& logger)
{
    spdlog::set_default_logger(logger);
}

} // namespace Rapid::System::Qt::Logger
