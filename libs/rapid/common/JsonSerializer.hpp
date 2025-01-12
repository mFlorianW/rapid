// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef JSONSERIALIZER_HPP
#define JSONSERIALIZER_HPP

#include "SessionData.hpp"

namespace Rapid::Common::JsonSerializer::Session
{

/**
 * Serialize the passed session meta data into a JSON string.
 * return string with JSON content.
 */
std::string serialize(SessionMetaData const& sessionMetaData);

/**
 * Serialize the passed session into a JSON string.
 * return string with JSON content.
 */
std::string serialize(SessionData const& session);

} // namespace Rapid::Common::JsonSerializer::Session

#endif // JSONSERIALIZER_HPP
