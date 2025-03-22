// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef JSONDESERIALIZER_HPP
#define JSONDESERIALIZER_HPP

#include "SessionData.hpp"
#include <optional>
#include <string>

namespace Rapid::Common::JsonDeserializer
{

namespace Session
{
std::optional<SessionData> deserialize(std::string const& rawData);
} // namespace Session

namespace SessionMetaData
{
std::optional<Common::SessionMetaData> deserialize(std::string const& rawData);
}

namespace Track
{
std::optional<Common::TrackData> deserialize(std::string rawData);
}

namespace Position
{
/**
     * @brief Tries to derserialize the JSON string into a @ref Rapid::Common::GpsPositionData
     *
     * @param rawData The raw JSON string
     *
     * @return std::optional<Common::GpsPositionData> with the Position or an nullopt
     */
std::optional<Common::GpsPositionData> deserialize(std::string rawData);
} // namespace Position

} // namespace Rapid::Common::JsonDeserializer

#endif // JSONDESERIALIZER_HPP
