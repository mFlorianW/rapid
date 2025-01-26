// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_SYSTEM_QT_RAPIDQAPPLICATION
#define RAPID_SYSTEM_QT_RAPIDQAPPLICATION

#include <QApplication>

namespace Rapid::System::Qt
{

/**
 * @brief Implements the @ref Rapid::System::EventLoop integration
 */
class RapidQApplication final : public QApplication
{
    Q_OBJECT
public:
    /**
     * Creates an instance and forwards the params to QApplication.
     * @param[in] argc The argument counter
     * @param[in] argv The arguments from the commond line
     */
    RapidQApplication(int& argc, char** argv);

    /**
     * Default destructor
     */
    ~RapidQApplication() override;
    Q_DISABLE_COPY_MOVE(RapidQApplication);
};

} // namespace Rapid::System::Qt

#endif // RAPID_SYSTEM_QT_RAPIDQAPPLICATION
