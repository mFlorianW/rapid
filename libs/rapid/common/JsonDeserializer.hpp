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

} // namespace Rapid::Common::JsonDeserializer

#endif // JSONDESERIALIZER_HPP
