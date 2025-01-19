// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef JSONSERIALIZER_HPP
#define JSONSERIALIZER_HPP

#include "SessionData.hpp"

namespace Rapid::Common::JsonSerializer
{

namespace Session
{

/**
 * @brief Serialize the passed session meta data into a JSON string.
 * @return string with JSON content.
 */
std::string serialize(SessionMetaData const& sessionMetaData);

/**
 * @brief Serialize the passed session into a JSON string.
 * @return string with JSON content.
 */
std::string serialize(SessionData const& session);

} // namespace Session

namespace Track
{

/**
 * @brief Serialize the passed track into a JSON string.
 * @return string with the JSON content.
 */
std::string serialize(TrackData const& trackData);

} // namespace Track

} // namespace Rapid::Common::JsonSerializer

#endif // JSONSERIALIZER_HPP
