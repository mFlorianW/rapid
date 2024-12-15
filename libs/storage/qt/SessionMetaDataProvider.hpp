// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SESSIONMETADATAPROVIDER_HPP
#define SESSIONMETADATAPROVIDER_HPP

#include "ISessionDatabase.hpp"
#include <SessionMetaData.hpp>
#include <TableModelDataProvider.hpp>

namespace Rapid::Storage::Qt
{

class SessionMetaDataProvider : public Common::Qt::TableModelDataProvider<Common::SessionMetaData>
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
