// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <QMainWindow>

namespace Ui
{
class SessionAnalyzer;
}

namespace Rapid::SessionAnalyzer
{

/**
 * @brief SessionAnalyzer application claas.
 * @details Brings up the main window make it possible to show and brings up the whole back-end.
 */
class SessionAnalyzer final : public QMainWindow
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(SessionAnalyzer)

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
    ~SessionAnalyzer() override;

private:
    std::unique_ptr<Ui::SessionAnalyzer> mMainWindow;
};

} // namespace Rapid::SessionAnalyzer
