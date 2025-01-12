// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ProcessStatusChecker.hpp"
#include <QProcess>

namespace Rapid::TestHelper
{

struct ProcessCheckStrategy
{
    ProcessCheckStrategy() = default;
    virtual ~ProcessCheckStrategy() = default;
    Q_DISABLE_COPY_MOVE(ProcessCheckStrategy)
    [[nodiscard]] virtual bool isProcessRunning(QString const& processName) const noexcept = 0;
};

namespace
{

#ifdef __linux__
struct LinuxProcessChecker : public ProcessCheckStrategy
{
    LinuxProcessChecker() = default;
    Q_DISABLE_COPY_MOVE(LinuxProcessChecker);
    ~LinuxProcessChecker() override = default;

    [[nodiscard]] bool isProcessRunning(QString const& processName) const noexcept override
    {
        auto pgrepProcess = QProcess{};
        pgrepProcess.setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);
        pgrepProcess.setProgram(QStringLiteral("pgrep"));
        pgrepProcess.setArguments({"-f", "-l", processName});
        pgrepProcess.start();
        pgrepProcess.waitForFinished(std::chrono::seconds{1000}.count());
        auto const result = QString{pgrepProcess.readAllStandardOutput()};
        return result.contains(processName);
    }
};
#elif _WIN32
// TODO add windows implementation
#endif

} // namespace

ProcessStatusChecker::ProcessStatusChecker()
#ifdef __linux__
    : mChecker{std::make_unique<LinuxProcessChecker>()}
#elif _WIN32
#endif
{
}

ProcessStatusChecker::~ProcessStatusChecker() = default;

bool ProcessStatusChecker::isProcessRunning(QString const& processName)
{
    return mChecker->isProcessRunning(processName);
}
} // namespace Rapid::TestHelper
