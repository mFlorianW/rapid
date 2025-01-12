// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationConfig.hpp"
#include <QDebug>
#include <QObject>
#include <QStringList>

namespace Rapid::RapidShell
{
ApplicationVersion::ApplicationVersion(qint32 major, qint32 minor, qint32 patch)
    : mMajor{major}
    , mMinor{minor}
    , mPatch{patch}
{
}
qint32 ApplicationVersion::getMajor() const noexcept
{
    return mMajor;
}

qint32 ApplicationVersion::getMinor() const noexcept
{
    return mMajor;
}

qint32 ApplicationVersion::getPatch() const noexcept
{
    return mMajor;
}

bool ApplicationVersion::operator==(ApplicationVersion const& other) const noexcept
{
    return (this == &other) or ((mMajor == other.mMajor) && (mMinor == other.mMinor) && (mPatch == other.mPatch));
}

bool ApplicationVersion::operator!=(ApplicationVersion const& other) const noexcept
{
    return !(*this == other);
}

ApplicationVersion ApplicationVersion::fromString(QString const& version) noexcept
{
    auto const splitedComponents = version.split(".");
    if (splitedComponents.size() < 3) {
        qCritical() << "Invalid version string passed. Error: string can't be splitted." << version;
        return ApplicationVersion{};
    }

    auto versions = std::array<qint32, 3>{};
    for (auto i = std::size_t{0}; i < versions.size(); ++i) {
        bool conversionSuccess = false;
        qint32 versionPart = splitedComponents.at(static_cast<qint32>(i)).toInt(&conversionSuccess);
        if (not conversionSuccess) {
            qCritical() << "Failed to convert version part " << i << "Error: conversion from string to int failed.";
            return ApplicationVersion{};
        } else {
            versions[i] = versionPart;
        }
    }

    return ApplicationVersion{versions[0], versions[1], versions[2]};
}

ApplicationConfig::ApplicationConfig(QString name, QString executable, QString iconUrl, ApplicationVersion appVersion)
    : mName{std::move(name)}
    , mExecutable{std::move(executable)}
    , mIconUrl{std::move(iconUrl)}
    , mVersion(std::move(appVersion))
{
}

QString ApplicationConfig::getName() const noexcept
{
    return mName;
}

QString ApplicationConfig::getExecutable() const noexcept
{
    return mExecutable;
}

QString ApplicationConfig::getIconUrl() const noexcept
{
    return mIconUrl;
}

ApplicationVersion ApplicationConfig::getAppVersion() const noexcept
{
    return mVersion;
}

bool ApplicationConfig::operator==(ApplicationConfig const& other) const noexcept
{
    return (this == &other) or ((mName == other.mName) && (mExecutable == other.mExecutable) &&
                                (mIconUrl == other.mIconUrl) && (mVersion == other.mVersion));
}

bool ApplicationConfig::operator!=(ApplicationConfig const& other) const noexcept
{
    return !(*this == other);
}
} // namespace Rapid::RapidShell
