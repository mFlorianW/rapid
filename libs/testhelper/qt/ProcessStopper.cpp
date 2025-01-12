// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ProcessStopper.hpp"
#ifdef __linux__
#include <QProcess>
#include <csignal>
#include <unistd.h>
#endif

namespace Rapid::TestHelper::ProcessStopper
{

#ifdef __linux__

bool stopProcess(std::string const& processName)
{
    auto pidOfProcess = QProcess{};
    pidOfProcess.setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);
    pidOfProcess.setProgram(QStringLiteral("pidof"));
    pidOfProcess.setArguments({QString::fromStdString(processName)});
    pidOfProcess.start();
    pidOfProcess.waitForFinished(std::chrono::seconds{1000}.count());
    auto result = QString{pidOfProcess.readAllStandardOutput()};
    auto ok = false;
    auto pid = static_cast<pid_t>(result.toUInt(&ok));
    if (ok and kill(pid, SIGTERM) == 0) {
        return true;
    }
    return false;
}

#endif

} // namespace Rapid::TestHelper::ProcessStopper
