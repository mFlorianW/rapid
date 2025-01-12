// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ApplicationListModel.hpp"
#include "ApplicationSearchDir.hpp"
#include "ProcessManager.hpp"
#include <QProcess>
#include <QQuickWidget>

namespace Rapid::RapidShell
{
class ApplicationOverviewWidget : public QQuickWidget
{
    Q_OBJECT
public:
    explicit ApplicationOverviewWidget(ProcessManager* proccessMgr);
    ~ApplicationOverviewWidget() override;
    Q_DISABLE_COPY_MOVE(ApplicationOverviewWidget)

    Q_INVOKABLE void startApplication(QString const& exe) noexcept;

private Q_SLOTS:
    void onQmlError(QQuickWindow::SceneGraphError const& error, QString const& message);

private:
    ApplicationModel mAppModel = ApplicationModel{{QString{INSTALL_SEARCH_DIR}}};
    ApplicationListModel mListModel = ApplicationListModel{mAppModel};
    QHash<QProcess*, std::shared_ptr<QProcess>> mProcesses;
    ProcessManager* mProcessManager;
};
} // namespace Rapid::RapidShell
