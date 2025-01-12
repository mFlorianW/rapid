// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

namespace Rapid::System
{

/**
 * The result of operation which may fail.
 */
enum class Result
{
    Ok, //< The opration is finshed sucessful
    Error, //< The operation failed
    NotFinished, //< The operation is not finished
};

} // namespace Rapid::System
