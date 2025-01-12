// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <QFileInfo>
#include <QStringList>

namespace Rapid::RapidShell
{
class ApplicationFinder
{
public:
    ApplicationFinder(QStringList appDirs);

    QVector<QFileInfo> findApplications() const noexcept;

private:
    QStringList mAppDirs;
};
} // namespace Rapid::RapidShell
