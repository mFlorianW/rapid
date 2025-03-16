// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LocalSessionManagement.hpp"

namespace Rapid::Workflow
{

LocalSessionManagement::LocalSessionManagement(Storage::ISessionDatabase* db)
    : mDb{db}
{
    assert(mDb != nullptr);
}

LocalSessionManagement::~LocalSessionManagement() = default;

std::shared_ptr<Storage::GetSessionResult> LocalSessionManagement::getSession(Common::SessionMetaData const& metadata)
{
    return mDb->getSessionByMetadataAsync(metadata);
}

} // namespace Rapid::Workflow
