// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationOverviewWidget.hpp"
#include <LoggingCategories.hpp>
#include <QQmlContext>
#include <QQmlEngine>

namespace Rapid::RapidShell
{

ApplicationOverviewWidget::ApplicationOverviewWidget(ProcessManager* proccessMgr)
    : mProcessManager{proccessMgr}
{
    Q_ASSERT(proccessMgr != nullptr);

    // Load initial applications
    mAppModel.loadApplications();
    // Set context properties for the application
    rootContext()->setContextProperty("g_applicationModel", &mListModel);
    rootContext()->setContextProperty("g_control", this);

    setResizeMode(QQuickWidget::SizeRootObjectToView);

    // Load QML file
    setSource(QUrl{"qrc:/qt/qml/Rapid/Shell/Applications/qml/ApplicationOverview.qml"});

    // Load initial error and display them
    auto const errorList = errors();
    for (auto const& error : errorList) {
        onQmlError(QQuickWindow::SceneGraphError::ContextNotAvailable, error.toString());
    }
}

ApplicationOverviewWidget::~ApplicationOverviewWidget()
{
    for (auto const& [key, process] : mProcesses.asKeyValueRange()) {
        process->terminate();
        process->waitForFinished(1000);
        if (process->state() == QProcess::Running) {
            process->kill();
        }
    }
}

Q_INVOKABLE void ApplicationOverviewWidget::startApplication(QString const& exe) noexcept
{
    if (not mProcessManager->isProcessRunning(exe)) {
        mProcessManager->startProcess(exe);
    }
}

void ApplicationOverviewWidget::onQmlError(QQuickWindow::SceneGraphError const& error, QString const& message)
{
    qCritical() << error << ":" << message;
}

} // namespace Rapid::RapidShell
