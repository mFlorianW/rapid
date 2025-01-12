// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ProcessManager.hpp"
#include <QFileInfo>
#include <spdlog/spdlog.h>

namespace Rapid::RapidShell
{

ProcessManager::ProcessManager() = default;

ProcessManager::~ProcessManager()
{
    for (auto const& [key, process] : mProcesses) {
        process->terminate();
        process->disconnect();
        process->waitForFinished(std::chrono::seconds{1}.count());
    }
}

void ProcessManager::startProcess(QString const& processBinaryPath)
{
    auto pro = std::make_unique<QProcess>();
    connect(pro.get(), &QProcess::errorOccurred, this, [this](QProcess::ProcessError const& error) {
        auto process = qobject_cast<QProcess*>(sender());
        if (process != nullptr and error == QProcess::ProcessError::FailedToStart) {
            auto error = ProcessManager::Error::ProcessNotFound;
            Q_EMIT processError(process->program(), error);
        }
    });
    connect(pro.get(), &QProcess::finished, this, [this](int exitCode) {
        auto process = qobject_cast<QProcess*>(sender());
        SPDLOG_INFO("Process {} stopped. With exit code: {}", process->program().toStdString(), exitCode);
        mProcesses.erase(QFileInfo{process->program()}.fileName());
    });
    pro->setProgram(processBinaryPath);
    pro->start();
    SPDLOG_INFO("Process application {} started", pro->program().toStdString());
    mProcesses.insert({QFileInfo{processBinaryPath}.fileName(), std::move(pro)});
}

bool ProcessManager::isProcessRunning(QString const& process) const noexcept
{
    auto fileInfo = QFileInfo{process};
    auto processName = process;
    if (fileInfo.isAbsolute()) {
        processName = fileInfo.fileName();
    }
    return mProcesses.find(processName) != mProcesses.cend();
}

} // namespace Rapid::RapidShell
