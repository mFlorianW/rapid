// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidAndroidModel.hpp"
#include <QCoreApplication>
#include <rest/qt/QRestClient.hpp>
#include <spdlog/spdlog.h>
#include <storage/SqliteSessionDatabase.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::Common::Qt;
using namespace Rapid::Rest;

namespace Rapid::Android
{

RapidAndroidModel::RapidAndroidModel()
{
    connect(&mDeviceManagement,
            &DeviceManagement::activeLaptimerChanged,
            this,
            &RapidAndroidModel::activeLaptimerChanged);
}
RapidAndroidModel::~RapidAndroidModel() = default;

Rapid::Common::Qt::DeviceSettings RapidAndroidModel::create(QString const& name,
                                                            QString const& ip,
                                                            QString port,
                                                            bool enabled) noexcept
{
    auto device = DeviceSettings{.name = name, .port = static_cast<quint16>(port.toUInt()), .defaultDevice = enabled};
    device.setIpAddress(ip);
    return device;
}

Rapid::Workflow::Qt::DeviceManagement* RapidAndroidModel::getDeviceManagement() noexcept
{
    return &mDeviceManagement;
}

Rapid::Common::Qt::DeviceSettings RapidAndroidModel::getActiveLaptimer() const noexcept
{
    return mDeviceManagement.getActiveLaptimer();
}

} // namespace Rapid::Android
