// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SESSION_HPP
#define SESSION_HPP

#include "common/SessionData.hpp"

namespace Rapid::TestHelper::Sessions
{
Common::SessionData getTestSession();
Common::SessionData getTestSession2();
Common::SessionData getTestSession3();
Common::SessionData getTestSession4();
char const* getTestSessionAsJson();

Common::SessionMetaData getTestSessionMetaData();
Common::SessionMetaData getTestSessionMetaData2();
char const* getTestSessionMetaAsJson();
char const* getTestSessionMetadataAsJson2();

} // namespace Rapid::TestHelper::Sessions

#endif // SESSION_HPP
