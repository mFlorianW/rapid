// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// clang-format off
#define QT_CATCH2_TEST_MAIN()  \
int main(int argc, char** argv) \
{ \
    Rapid::System::Qt::RapidApplication app(argc, argv); \
    int result = Catch::Session().run(argc, argv); \
    return result; \
}
// clang-format on
// NOLINTEND(cppcoreguidelines-macro-usage)
