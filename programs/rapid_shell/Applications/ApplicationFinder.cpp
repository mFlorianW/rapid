// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationFinder.hpp"
#include <QDirIterator>
#include <QFileInfo>

namespace Rapid::RapidShell
{

ApplicationFinder::ApplicationFinder(QStringList appDirs)
    : mAppDirs{std::move(appDirs)}
{
}

QVector<QFileInfo> ApplicationFinder::findApplications() const noexcept
{
    auto applications = QVector<QFileInfo>{};
    for (auto const& appDir : mAppDirs) {
        auto dirIter = QDirIterator{appDir, QDirIterator::NoIteratorFlags | QDirIterator::Subdirectories};
        while (dirIter.hasNext()) {
            dirIter.next();
            if (dirIter.fileInfo().isFile() and dirIter.fileInfo().completeSuffix() == QStringLiteral("json")) {
                applications.push_back(dirIter.fileInfo());
            }
        }
    }

    return applications;
}

} // namespace Rapid::RapidShell
