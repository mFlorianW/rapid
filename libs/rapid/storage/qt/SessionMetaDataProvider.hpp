// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SESSIONMETADATAPROVIDER_HPP
#define SESSIONMETADATAPROVIDER_HPP

#include "storage/ISessionDatabase.hpp"
#include <common/SessionMetaData.hpp>
#include <common/qt/SessionMetadataProvider.hpp>

namespace Rapid::Storage::Qt
{

class SessionMetaDataProvider : public Common::Qt::SessionMetadataProvider
{
public:
    SessionMetaDataProvider(ISessionDatabase& sessionDb);

private:
    void requestSessionMetaData(std::size_t index) noexcept;
    void handleSessionMetaDataRequest(System::AsyncResult* self, std::size_t index) noexcept;
    using MetaDataRequstCache = std::unordered_map<System::AsyncResult*, std::shared_ptr<GetSessionMetaDataResult>>;
    ISessionDatabase& mSessionDb;
    MetaDataRequstCache mRequestCache;
};

} // namespace Rapid::Storage::Qt

#endif // SESSIONMETADATAPROVIDER_HPP
