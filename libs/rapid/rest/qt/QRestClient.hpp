// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "rest/IRestClient.hpp"
#include <QObject>
#include <QtGlobal>

namespace Rapid::Rest
{
struct QRestClientPrivate;

/**
 * @copydoc Rapid::Rest::IRestClient
 */
class QRestClient : public IRestClient
{
    Q_GADGET
public:
    Q_DISABLE_COPY_MOVE(QRestClient)

    /**
     * Creates an instance of the @ref QRestClient
     */
    QRestClient();

    /**
     * Default destructor
     */
    ~QRestClient() noexcept override;

    /**
     * @copydoc Rapid::Rest::IRestClient::setServerAddress
     */
    void setServerAddress(std::string const& url) noexcept override;

    /**
     * @copydoc Rapid::Rest::IRestClient::setServerPort
     */
    void setServerPort(std::uint16_t port) noexcept override;

    /**
     * @copydoc Rapid::Rest::IRestClient::execute
     */
    std::shared_ptr<RestCall> execute(RestRequest const& request) noexcept override;

private:
    std::unique_ptr<QRestClientPrivate> d;
};

} // namespace Rapid::Rest
