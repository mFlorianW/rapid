// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "rest/IRestClient.hpp"
#include <QtGlobal>

namespace Rapid::Rest
{

struct QRestClientPrivate;
class QRestClient : public IRestClient
{
    Q_DISABLE_COPY_MOVE(QRestClient)
public:
    QRestClient();

    ~QRestClient() noexcept override;

    void setServerAddress(std::string const& url) noexcept override;

    void setServerPort(std::uint16_t port) noexcept override;

    std::shared_ptr<RestCall> execute(RestRequest const& request) noexcept override;

private:
    std::unique_ptr<QRestClientPrivate> d;
};

} // namespace Rapid::Rest
