// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "ApplicationConfig.hpp"
#include <QStringList>
#include <QVector>

namespace Rapid::RapidShell
{
class ApplicationModel
{
public:
    ApplicationModel(QStringList applicationFolders = {}) noexcept;
    void loadApplications() noexcept;
    [[nodiscard]] QVector<ApplicationConfig> getApplications() const noexcept;

private:
    QVector<ApplicationConfig> mApps;
    QStringList mAppFolders;
};

} // namespace Rapid::RapidShell
