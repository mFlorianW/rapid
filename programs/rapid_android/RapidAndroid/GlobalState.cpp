// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GlobalState.hpp"
#include "Database.hpp"
#include <QCoreApplication>
#include <rest/qt/QRestClient.hpp>
#include <spdlog/spdlog.h>
#include <storage/SqliteSessionDatabase.hpp>

using namespace Rapid::Workflow::Qt;
using namespace Rapid::Common::Qt;
using namespace Rapid::Rest;

namespace Rapid::Android
{

GlobalState::GlobalState()
    : mDeviceManagement{new DeviceManagement{std::addressof(mSettingsBackend)}}
{
    auto dbFile = setupDatabase();
    if (not dbFile.has_value()) {
        SPDLOG_CRITICAL("Faild to setup the database. Exiting!");
        QCoreApplication::exit(-1);
        return;
    }
    mSessionDatabase = std::make_unique<Storage::SqliteSessionDatabase>(dbFile.value().toStdString());
    connect(mDeviceManagement, &DeviceManagement::activeLaptimerChanged, this, [this] {
        setRestClient();
        mRestActiveSession->setRestClient(mRestClient.get());
    });
    setRestClient();
    mRestActiveSession = std::make_unique<RestActiveSession>(mRestClient.get());
    mLocalSessionManagement = std::make_unique<LocalSessionManagement>(mSessionDatabase.get());
}

GlobalState::~GlobalState()
{
    delete mDeviceManagement;
}

Rapid::Workflow::Qt::RestActiveSession const* GlobalState::getActiveSession() const noexcept
{
    return mRestActiveSession.get();
}

Rapid::Workflow::Qt::LocalSessionManagement* GlobalState::getLocalSessionManagement() const noexcept
{
    return mLocalSessionManagement.get();
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

void GlobalState::setRestClient()
{
    auto const activeLaptimer = mDeviceManagement->getActiveLaptimer();
    mRestClient = std::make_unique<QRestClient>();
    mRestClient->setServerAddress(activeLaptimer.getIpAddress().toStdString());
    mRestClient->setServerPort(activeLaptimer.port);
}

} // namespace Rapid::Android
