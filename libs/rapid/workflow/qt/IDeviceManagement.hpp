// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_QT_IDEVICEMANAGEMENT
#define RAPID_WORKFLOW_QT_IDEVICEMANAGEMENT

#include <QObject>

namespace Rapid::Workflow::Qt
{

/**
 * @brief Interface to manage the devies inside the application
 *
 * @details This interface defines the functions to manage devices (laptimer) inside the application
 *          Those functions are loading, saving, displaying and editing.
 */
class IDeviceManagement : public QObject
{
    Q_OBJECT

public:
    /**
     * Default destructor
     */
    ~IDeviceManagement() override = default;

    /**
     * Disabled copy constructor
     */
    IDeviceManagement(IDeviceManagement const&) = delete;

    /**
     * Disablbed copy assignment operator
     */
    IDeviceManagement& operator=(IDeviceManagement const&) = delete;

    /**
     * Disabled move constructor
     */
    IDeviceManagement(IDeviceManagement&&) noexcept = delete;

    /**
     * Disablbed move assignment operator
     */
    IDeviceManagement& operator=(IDeviceManagement&&) noexcept = delete;

protected:
    /**
     * Default constructor
     */
    IDeviceManagement() = default;
};

} // namespace Rapid::Workflow::Qt

#endif // !RAPID_WORKFLOW_QT_IDEVICEMANAGEMENT
