// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QQmlApplicationEngine>

namespace Rapid::SessionAnalyzer
{

/**
 * @brief SessionAnalyzer application claas.
 * @details Brings up the main window make it possible to show and brings up the whole back-end.
 */
class SessionAnalyzer final
{
public:
    /**
     * @brief Creates an instance of @ref SessionAnalyzer
     *
     * @details Setups the main window and back-end
     *          There is only one instance of that class created in the main function.
     */
    SessionAnalyzer();

    /**
     * Default destructor
     */
    ~SessionAnalyzer() = default;

    Q_DISABLE_COPY_MOVE(SessionAnalyzer)

    /**
     * @brief Shows the main window of the session analyzer.
     *
     * @details Requests the top level window from the QML engine and shows it.
     */
    void show() const noexcept;

private:
    QQmlApplicationEngine mEngine{};
};

} // namespace Rapid::SessionAnalyzer
