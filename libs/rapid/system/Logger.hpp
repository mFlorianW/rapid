// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_SYSTEM_LOGGER_HPP
#define RAPID_SYSTEM_LOGGER_HPP

#include <spdlog/spdlog.h>

namespace Rapid::System::Logger
{

/**
 * @brief Set default logger for the Rapid library useful e.g. Android build
 *
 * @param logger The default logger
 */
void setDefaultLogger(std::shared_ptr<spdlog::logger> const& logger);

} // namespace Rapid::System::Logger

#endif // !RAPID_SYSTEM_LOGGER_HPP
