// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QProcess>
#include <QString>
#include <memory>
#include <unordered_map>

namespace Rapid::RapidShell
{
/**
 * The @ref ProcessManager tracks all started processes by the shell.
 * It notifes about the process changes with by @ref ProcessManager::processStarted and @ref ProcessManager::stop.
 * An error of a process is reported with the @ref ProcessManager::proccessError signal.
 */
class ProcessManager : public QObject
{
    Q_OBJECT
public:
    enum Error
    {
        ProcessNotFound,
    };

    /**
     * Creates an Instance of the ProcessManager
     */
    ProcessManager();

    /**
     * On destruction every process is stopped or killed.
     */
    ~ProcessManager() override;

    Q_DISABLE_COPY_MOVE(ProcessManager)

    /**
     * Tries to start a process with under the passed process binary path.
     * The path should be absolute the ProcessManager doesn't make any lookups in e.g. PATH env variable.
     * Errors of the startup of the process are reported with @ref ProcessManager::ProcessError signal.
     * @processBinaryPath The absolute path of the binary that shall be started.
     */
    void startProcess(QString const& processBinaryPath);

    /**
     * Checks if a process is running that is managed
     * @param process The name of the process that shall be checked if it runs.
     *                This can be only the process name or the absolute process binary path.
     * @return true means the process is running otherwise false.
     */
    [[nodiscard]] bool isProcessRunning(QString const& process) const noexcept;

Q_SIGNALS:
    void processError(QString const& processName, ProcessManager::Error error);

private:
    std::unordered_map<QString, std::unique_ptr<QProcess>> mProcesses;
};

} // namespace Rapid::RapidShell
