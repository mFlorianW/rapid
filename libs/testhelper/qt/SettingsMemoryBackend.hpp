// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <common/qt/GlobalSettingsBackend.hpp>

namespace Rapid::TestHelper
{

class SettingsMemoryBackend : public Common::Qt::GlobalSettingsBackend
{
public:
    [[nodiscard]] bool storeValue(QAnyStringView const& key, QVariant const& value) noexcept override;

    [[nodiscard]] QVariant getValue(QAnyStringView const& key) const noexcept override;

private:
    QVariantHash mSingleValues;
};

} // namespace Rapid::TestHelper
