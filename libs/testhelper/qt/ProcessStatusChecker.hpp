// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QString>

namespace Rapid::TestHelper
{
struct ProcessCheckStrategy;
class ProcessStatusChecker
{
public:
    ProcessStatusChecker();
    ~ProcessStatusChecker();

    Q_DISABLE_COPY_MOVE(ProcessStatusChecker);

    [[nodiscard]] bool isProcessRunning(QString const& processName);

private:
    std::unique_ptr<ProcessCheckStrategy> mChecker;
};

} // namespace Rapid::TestHelper
