// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QHostAddress>
#include <QQmlEngine>
#include <fmt/format.h>

namespace Rapid::Common::Qt
{

/**
 * Global Device settings for a laptimer.
 */
class DeviceSettings
{
    Q_GADGET
    QML_VALUE_TYPE(deviceSettings)

    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString ip READ getIpAddress WRITE setIpAddress)
    Q_PROPERTY(quint16 port MEMBER port)

public:
    QString name;
    QHostAddress ip;
    quint16 port;
    bool defaultDevice = false;

    QString getIpAddress() const noexcept
    {
        return ip.toString();
    }

    void setIpAddress(QString const ipString) noexcept
    {
        auto const address = QHostAddress{ipString};
        if (not address.isNull()) {
            ip = address;
        }
    }

    bool operator==(DeviceSettings const& rhs) const = default;
    bool operator!=(DeviceSettings const& rhs) const = default;
};

} // namespace Rapid::Common::Qt

template <>
struct fmt::formatter<Rapid::Common::Qt::DeviceSettings>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(Rapid::Common::Qt::DeviceSettings const& obj, FormatContext& ctx) const noexcept
    {
        return fmt::format_to(ctx.out(),
                              "Lpatimer(name={}, ip={}, port={})",
                              obj.name.toStdString(),
                              obj.getIpAddress().toStdString(),
                              obj.port);
    }
};
