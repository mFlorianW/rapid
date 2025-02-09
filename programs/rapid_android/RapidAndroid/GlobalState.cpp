// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GlobalState.hpp"

using namespace Rapid::Workflow::Qt;
using namespace Rapid::Common::Qt;

namespace Rapid::Android
{

GlobalState::GlobalState()
    : mDeviceManagement{new DeviceManagement{std::addressof(mSettingsBackend)}}
{
}

GlobalState::~GlobalState()
{
    delete mDeviceManagement;
}

Rapid::Common::Qt::DeviceSettings GlobalState::create(QString const& name,
                                                      QString const& ip,
                                                      QString port,
                                                      bool enabled) noexcept
{
    auto device = DeviceSettings{.name = name, .port = static_cast<quint16>(port.toUInt()), .defaultDevice = enabled};
    device.setIpAddress(ip);
    return device;
}

} // namespace Rapid::Android
