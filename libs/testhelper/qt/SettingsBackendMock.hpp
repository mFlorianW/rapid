// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SETTINGSBACKENDMOCK_HPP
#define SETTINGSBACKENDMOCK_HPP

#include <common/qt/GlobalSettingsBackend.hpp>
#include <trompeloeil.hpp>

namespace Rapid::TestHelper
{

class SettingsBackendMock : public Rapid::Common::Qt::GlobalSettingsBackend
{
public:
    MAKE_MOCK(getValue, auto(QAnyStringView const& key)->QVariant, const noexcept override);
    MAKE_MOCK(storeValue, auto(QAnyStringView const& key, QVariant const& value)->bool, noexcept override);
};

} // namespace Rapid::TestHelper

#endif // SETTINGSBACKENDMOCK_HPP
