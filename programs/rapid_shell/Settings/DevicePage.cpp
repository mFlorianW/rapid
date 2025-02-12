// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DevicePage.hpp"
#include "ui_DevicePage.h"

namespace Rapid::RapidShell::Settings
{

DevicePage::DevicePage(Common::Qt::Private::GlobalSettingsWriter* settingsWriter,
                       Common::Qt::GlobalSettingsReader* settingsReader)
    : mDeviceModel{settingsWriter, settingsReader}
    , mDevicePage{std::make_unique<Ui::DevicePage>()}
{
    // Setup device page
    mDevicePage->setupUi(this);

    // Setup header view
    mHeaderView.setModel(&mDeviceModel);
    mHeaderView.setStretchLastSection(true);

    // Setup device model
    mDevicePage->deviceTable->setHorizontalHeader(&mHeaderView);
    mDevicePage->deviceTable->setModel(&mDeviceModel);

    // Setup Add Handler
    connect(mDevicePage->addButton, &QPushButton::clicked, this, [this]() {
        mDeviceModel.insertRows(mDeviceModel.rowCount({}));
    });

    // Setup Remove Handler
    connect(mDevicePage->removeButton, &QPushButton::clicked, this, [this]() {
        auto const selectionModel = mDevicePage->deviceTable->selectionModel();
        if (selectionModel->hasSelection()) {
            auto const selectedIndex = mDevicePage->deviceTable->selectionModel()->selectedIndexes().first();
            if (selectedIndex.isValid()) {
                mDeviceModel.removeRows(selectedIndex.row());
            }
        }
    });
}

DevicePage::~DevicePage() = default;

bool DevicePage::restore()
{
    return mDeviceModel.restore();
}

bool DevicePage::save()
{
    return mDeviceModel.save();
}

} // namespace Rapid::RapidShell::Settings
