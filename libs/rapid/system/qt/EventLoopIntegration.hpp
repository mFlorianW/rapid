// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

namespace Rapid::System::Qt::EventLoopIntegration
{

/**
 * @brief Qt Eventloop integration.
 *
 * @details Makes the Qt event loop integration.
 *          This function must be called from every thread that shall handle events from @ref Rapid::System::EventLoop.
 *          The function has to be called after the creation of the Qt event loop.
 *
 *  @note
 *          For the main loop of a Qt application use one of the:
 *              - @ref Rapid::System::Qt::RapidCoreApplication
 *              - @ref Rapid::System::Qt::RapidGuiApplication
 *              - @ref Rapid::System::Qt::RapidApplication
 *
 * @return True the integration successful happens.
 * @return False If not event loop is present from the thread it's called.
 */
[[nodiscard]] extern bool makeEventLoopIntegration();

} // namespace Rapid::System::Qt::EventLoopIntegration
