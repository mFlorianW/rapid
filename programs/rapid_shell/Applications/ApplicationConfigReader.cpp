// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ApplicationConfigReader.hpp"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace Rapid::RapidShell
{

std::optional<ApplicationConfig> ApplicationConfigReader::readConfig(QString const& configPath) const noexcept
{
    if (not QFile::exists(configPath)) {
        qCritical() << "Failed to load application config. Error: Configuration not found. File:" << configPath;
        return {};
    }

    auto configFile = QFile{configPath};
    if (not configFile.open(QFile::ReadOnly)) {
        qCritical() << "Failed to load application config. Error: Configuration cannot be open. File:" << configPath;
        return {};
    }

    auto jsonError = QJsonParseError{};
    auto const jsonDoc = QJsonDocument::fromJson(configFile.readAll(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qCritical() << "Failed to load JSON document. Error:" << jsonError.error << "File:" << configPath;
        return {};
    }

    if (not jsonDoc.isObject()) {
        qCritical() << "Inavlid JSON document. Error: Json document object not found. File:" << configPath;
        return {};
    }

    auto const jsonObj = jsonDoc.object();
    if (jsonObj.find("name") == jsonObj.end() or jsonObj.find("executable") == jsonObj.end() or
        jsonObj.find("iconUrl") == jsonObj.end() or jsonObj.find("version") == jsonObj.end()) {
        qCritical() << "Inavlid JSON document. Error: Required JSON keys not found. File:" << configPath;
        return {};
    }

    return ApplicationConfig(jsonObj.value("name").toString(),
                             jsonObj.value("executable").toString(),
                             jsonObj.value("iconUrl").toString(),
                             ApplicationVersion::fromString(jsonObj.value("version").toString()));
}

} // namespace Rapid::RapidShell
