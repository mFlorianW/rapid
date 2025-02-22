// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "QRestClient.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <spdlog/spdlog.h>

namespace Rapid::Rest
{

class QRestCall final : public QObject, public RestCall
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QRestCall)
public:
    QRestCall(QNetworkReply& reply)
        : mRequest{reply}
    {
        connect(&mRequest, &QNetworkReply::finished, this, &QRestCall::callFinished);
        connect(&mRequest, &QNetworkReply::errorOccurred, this, &QRestCall::callFinishedWithError);
    }

    ~QRestCall() noexcept override = default;

protected:
    void callFinished() noexcept
    {
        setData(mRequest.readAll().toStdString());
        setCallResult(RestCallResult::Success);
    }

    void callFinishedWithError() noexcept
    {
        qCritical() << "Call to" << mRequest.url() << "finished with error:" << mRequest.errorString();
        setCallResult(RestCallResult::Error);
    }

    void setCallResult(RestCallResult result) noexcept override
    {
        try {
            mRequest.deleteLater();
            mResult = result;
            finished.emit(this);
        } catch (std::exception const& e) {
            SPDLOG_ERROR("Failed to emit finished signal. Error: Signal is already emitting.");
        }
    }

private:
    QNetworkReply& mRequest;
};

struct QRestClientPrivate
{
    std::string mServerAddress;
    std::uint16_t mPort{0};
    QNetworkAccessManager mNetworkAccessManager;

    QUrl getUrl() const noexcept
    {
        auto url = QUrl{};
        url.setHost(QString::fromStdString(mServerAddress));
        url.setPort(mPort);
        return url;
    }
};

QRestClient::QRestClient()
    : d{std::make_unique<QRestClientPrivate>()}
{
}

QRestClient::~QRestClient() noexcept = default;

void QRestClient::setServerAddress(std::string const& url) noexcept
{
    d->mServerAddress = url;
}

void QRestClient::setServerPort(std::uint16_t port) noexcept
{
    d->mPort = port;
}

std::shared_ptr<RestCall> QRestClient::execute(RestRequest const& request) noexcept
{
    if (request.getType() == RequestType::Get) {
        auto path = QString::fromStdString(std::string{request.getPath().getPath()});
        auto url = d->getUrl();
        url.setScheme(QStringLiteral("http"));
        url.setPath(path);
        auto networrkRequest = QNetworkRequest{};
        networrkRequest.setUrl(url);
        auto const reply = d->mNetworkAccessManager.get(networrkRequest);
        return std::make_shared<QRestCall>(*reply);
    } else if (request.getType() == RequestType::Post) {
        qInfo() << "Not implemented REST request type post";
    } else if (request.getType() == RequestType::Delete) {
        qInfo() << "Not implemented REST request type delete";
    }

    return nullptr;
}

} // namespace Rapid::Rest

// NOLINTNEXTLINE
#include "QRestClient.moc"
