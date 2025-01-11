// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QApplication>

namespace Rapid::System::Qt
{

/**
 * @brief Implements the @ref Rapid::System::EventLoop integration
 */
class RapidCoreApplication final : public QCoreApplication
{
    Q_OBJECT
public:
    /**
     * Creates an instance and forwards the params to QCoreApplication.
     * @param[in] argc The argument counter
     * @param[in] argv The arguments from the commond line
     */
    RapidCoreApplication(int& argc, char** argv);

    /**
     * Default destructor
     */
    ~RapidCoreApplication() override;
    Q_DISABLE_COPY_MOVE(RapidCoreApplication);
};

/**
 * @brief Implements the @ref Rapid::System::EventLoop integration
 */
class RapidGuiApplication final : public QGuiApplication
{
    Q_OBJECT
public:
    /**
     * Creates an instance and forwards the params to QCoreApplication.
     * @param[in] argc The argument counter
     * @param[in] argv The arguments from the commond line
     */
    RapidGuiApplication(int& argc, char** argv);

    /**
     * Default destructor
     */
    ~RapidGuiApplication() override;
    Q_DISABLE_COPY_MOVE(RapidGuiApplication);
};

/**
 * @brief Implements the @ref Rapid::System::EventLoop integration
 */
class RapidApplication final : public QApplication
{
    Q_OBJECT
public:
    /**
     * Creates an instance and forwards the params to QApplication.
     * @param[in] argc The argument counter
     * @param[in] argv The arguments from the commond line
     */
    RapidApplication(int& argc, char** argv);

    /**
     * Default destructor
     */
    ~RapidApplication() override;
    Q_DISABLE_COPY_MOVE(RapidApplication);
};

} // namespace Rapid::System::Qt