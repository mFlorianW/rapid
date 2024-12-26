// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <QMainWindow>

namespace Ui
{
class SessionManager;
}

namespace Rapid::SessionManager
{

/**
 * @brief SessionManager application claas.
 * @details Brings up the main window make it possible to show and brings up the whole back-end.
 */
class SessionManager final : public QMainWindow
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(SessionManager);

    /**
     * @brief Creates an instance of @ref SessionManager
     *
     * @details Setups the main window and back-end
     *          There is only one instance of that class created in the main function.
     */
    SessionManager();

    /**
     * Default destructor
     */
    ~SessionManager() override;

private:
    std::unique_ptr<Ui::SessionManager> mSessionManager;
};

} // namespace Rapid::SessionManager

#endif //SESSIONMANAGER_HPP
