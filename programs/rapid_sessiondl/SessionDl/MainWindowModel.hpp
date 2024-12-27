// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QHostAddress>
#include <QObject>
#include <rest/qt/QRestClient.hpp>
#ifndef Q_MOC_RUN
#include <ISessionDatabase.hpp>
#include <RestSessionDownloader.hpp>
#endif

namespace Rapid::SessionDl
{
class MainWindowModel : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MainWindowModel)

    Q_PROPERTY(QString hostAddress READ getHostAddress WRITE setHostAddress NOTIFY hostAddressChanged)

    Q_PROPERTY(QString hostPort READ getHostPort WRITE setHostPort NOTIFY hostPortChanged)

    Q_PROPERTY(QString logMessage READ getLogMessage NOTIFY logMessageChanged)
public:
    MainWindowModel(Workflow::ISessionDownloader& downloader, Storage::ISessionDatabase& database) noexcept;
    ~MainWindowModel() noexcept override;

    [[nodiscard]] QString getHostAddress() const noexcept;
    void setHostAddress(QString const& hostAddress);
    void setHostAddress(QHostAddress const& hostAddress);

    [[nodiscard]] QString getHostPort() const noexcept;
    void setHostPort(QString const& hostPort);
    void setHostPort(quint16 hostPort);

    Q_INVOKABLE void startSessionDownload() noexcept;

    QString getLogMessage() const noexcept;

Q_SIGNALS:
    void hostAddressChanged();
    void hostPortChanged();
    void logMessageChanged();

private Q_SLOTS:
    void clearLog() noexcept;
    void appendToLog(QString const& message) noexcept;

private:
    QHostAddress mHostAddress;
    quint16 mHostPort{0};
    QString mDownloadLog;
    Workflow::ISessionDownloader& mSessionDownloader;
    Storage::ISessionDatabase& mSessionDatabase;
    std::unordered_map<System::AsyncResult*, std::shared_ptr<System::AsyncResult>> mStorageCalls;
};
} // namespace Rapid::SessionDl
