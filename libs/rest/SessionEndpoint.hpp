// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IRestRequestHandler.hpp"
#include "ISessionDatabase.hpp"

namespace Rapid::Rest
{

class SessionEndpoint : public IRestRequestHandler
{
public:
    SessionEndpoint(Storage::ISessionDatabase& database) noexcept;

    void handleRestRequest(RestRequest& request) noexcept override;

private:
    void handleGetRequest(RestRequest& request) noexcept;
    void handleDeleteRequest(RestRequest& request) noexcept;
    void onSessionResult(System::AsyncResult* result);

private:
    Storage::ISessionDatabase& mDb;
    struct SessionGetDataRequest
    {
        std::shared_ptr<Storage::GetSessionResult> sessionResult;
        RestRequest request;
    };
    std::unordered_map<System::AsyncResult*, SessionGetDataRequest> mGetSessionRequests;
};

} // namespace Rapid::Rest
