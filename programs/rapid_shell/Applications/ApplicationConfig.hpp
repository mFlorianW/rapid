// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <QString>

namespace Rapid::RapidShell
{
class ApplicationVersion final
{
public:
    ApplicationVersion() = default;
    ApplicationVersion(qint32 mMajor, qint32 mMinor, qint32 mPatch);

    qint32 getMajor() const noexcept;

    qint32 getMinor() const noexcept;

    qint32 getPatch() const noexcept;

    bool operator==(ApplicationVersion const& other) const noexcept;
    bool operator!=(ApplicationVersion const& other) const noexcept;

    static ApplicationVersion fromString(QString const& version) noexcept;

private:
    qint32 mMajor{0};
    qint32 mMinor{0};
    qint32 mPatch{0};
};

class ApplicationConfig
{
public:
    ApplicationConfig(QString name, QString executable, QString iconUrl, ApplicationVersion appVersion);

    QString getName() const noexcept;

    QString getExecutable() const noexcept;

    QString getIconUrl() const noexcept;

    ApplicationVersion getAppVersion() const noexcept;

    bool operator==(ApplicationConfig const& other) const noexcept;
    bool operator!=(ApplicationConfig const& other) const noexcept;

private:
    QString mName;
    QString mExecutable;
    QString mIconUrl;
    ApplicationVersion mVersion;
};

} // namespace Rapid::RapidShell
