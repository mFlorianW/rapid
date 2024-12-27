// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QHostAddress>

namespace Rapid::Common::Qt
{

/**
 * Global Device settings for a laptimer.
 */
struct DeviceSettings
{
    QString name;
    QHostAddress ip;
    quint16 port;
    bool defaultDevice = false;
};

} // namespace Rapid::Common::Qt
