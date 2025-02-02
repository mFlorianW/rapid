// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceManagement.hpp"
#include <common/qt/GenericTableModel.hpp>

namespace Rapid::Workflow::Qt
{

DeviceManagement::DeviceManagement(Common::Qt::GlobalSettingsBackend* settingsBackend)
    : IDeviceManagement{}
    , mGlobalSettingsReader{std::make_unique<Common::Qt::GlobalSettingsReader>(settingsBackend)}
    , mDeviceSettingsProvider{std::make_unique<Common::Qt::DeviceSettingsProvider>(*mGlobalSettingsReader)}
    , mModel{
          std::make_unique<Common::Qt::GenericTableModel<Common::Qt::DeviceSettingsProvider>>(*mDeviceSettingsProvider)}
{
}

DeviceManagement::~DeviceManagement() = default;

QAbstractItemModel const* const DeviceManagement::getModel() const noexcept
{
    return mModel.get();
}

} // namespace Rapid::Workflow::Qt
