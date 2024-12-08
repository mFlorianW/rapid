// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MainWindowModel.hpp"
#include <QDateTime>

namespace Rapid::SessionDl
{

MainWindowModel::MainWindowModel(Workflow::ISessionDownloader& downloader, Storage::ISessionDatabase& database) noexcept
    : mSessionDownloader(downloader)
    , mSessionDatabase(database)
{
    std::ignore = mSessionDownloader.sessionCountFetched.connect([this] {
        appendToLog(QString("Found %1 session on the device").arg(mSessionDownloader.getSessionCount()));
        appendToLog(QStringLiteral("Fetched session count from device"));

        for (std::size_t i = 0; i < mSessionDownloader.getSessionCount(); ++i) {
            mSessionDownloader.downloadSession(i);
            appendToLog(QString{"Start session download from %1 of %2"}
                            .arg(QString::number(i + 1))
                            .arg(static_cast<qint32>(mSessionDownloader.getSessionCount())));
        }
    });

    std::ignore =
        mSessionDownloader.sessionDownloadFinshed.connect([this](std::size_t index, Workflow::DownloadResult result) {
            appendToLog(QString{"Downloaded session %1 of %2 from Device"}
                            .arg(QString::number(index + 1))
                            .arg(static_cast<qint32>(mSessionDownloader.getSessionCount())));

            if (result == Workflow::DownloadResult::Ok) {
                appendToLog(QString{"Store Session %1 of %2 in database"}
                                .arg(QString::number(index + 1))
                                .arg(static_cast<qint32>(mSessionDownloader.getSessionCount())));
                auto session = mSessionDownloader.getSession(index);
                if (session) {
                    auto asyncResult = mSessionDatabase.storeSession(session.value());
                    auto resultHandler = [this, index](System::AsyncResult* result) {
                        auto logString = result->getResult() == System::Result::Ok
                                             ? QString{"Session stored %1 of %2 in database"}
                                                   .arg(QString::number(index + 1))
                                                   .arg(static_cast<qint32>(mSessionDownloader.getSessionCount()))
                                             : QString{"Failed to store %1 of %2 in database. Error:%3"}
                                                   .arg(QString::number(index + 1))
                                                   .arg(static_cast<qint32>(mSessionDownloader.getSessionCount()))
                                                   .arg(QString::fromStdString(std::string{result->getErrorMessage()}));
                        appendToLog(logString);
                        mStorageCalls.erase(result);
                        if (mStorageCalls.empty()) {
                            appendToLog("All sessions downloaded!");
                        }
                    };
                    std::ignore = asyncResult->done.connect(resultHandler);
                    mStorageCalls.insert({asyncResult.get(), asyncResult});
                }
            }
        });
}

MainWindowModel::~MainWindowModel() noexcept = default;

QString MainWindowModel::getHostAddress() const noexcept
{
    return mHostAddress.toString();
}

void MainWindowModel::setHostAddress(QString const& hostAddress)
{
    auto const newHost = QHostAddress{hostAddress};
    if (mHostAddress != newHost) {
        mHostAddress = newHost;
        Q_EMIT hostAddressChanged();
    }
}

void MainWindowModel::setHostAddress(QHostAddress const& hostAddress)
{
    if (mHostAddress != hostAddress) {
        mHostAddress = hostAddress;
        Q_EMIT hostAddressChanged();
    }
}

QString MainWindowModel::getHostPort() const noexcept
{
    return QString::number(mHostPort);
}

void MainWindowModel::setHostPort(QString const& hostPort)
{
    auto const newPort = hostPort.toShort();
    if (mHostPort != newPort) {
        mHostPort = newPort;
        Q_EMIT hostPortChanged();
    }
}

void MainWindowModel::setHostPort(quint16 hostPort)
{
    if (mHostPort != hostPort) {
        mHostPort = hostPort;
        Q_EMIT hostPortChanged();
    }
}

void MainWindowModel::startSessionDownload() noexcept
{
    clearLog();
    appendToLog(QStringLiteral("Begin Session Download"));
    appendToLog(QStringLiteral("Fetch Session Count from Device"));
    mSessionDownloader.fetchSessionCount();
}

QString MainWindowModel::getLogMessage() const noexcept
{
    return mDownloadLog;
}

void MainWindowModel::clearLog() noexcept
{
    mDownloadLog.clear();
    Q_EMIT logMessageChanged();
}

void MainWindowModel::appendToLog(QString const& message) noexcept
{
    auto const logMsg = QString("%1: %2\n").arg(QDateTime::currentDateTime().toString(), message);
    mDownloadLog.append(logMsg);
    Q_EMIT logMessageChanged();
}

} // namespace Rapid::SessionDl
