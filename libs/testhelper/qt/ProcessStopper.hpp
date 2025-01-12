// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <string>

namespace Rapid::TestHelper::ProcessStopper
{
/**
 * Tries to stop the process for the passed name.
 * @return True the process for the name was stopped otherwise false.
 */
bool stopProcess(std::string const& process);

} // namespace Rapid::TestHelper::ProcessStopper
