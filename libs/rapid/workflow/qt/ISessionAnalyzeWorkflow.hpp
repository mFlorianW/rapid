// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef ISESSIONANALYZEWORKFLOW_HPP
#define ISESSIONANALYZEWORKFLOW_HPP

#include <common/SessionData.hpp>
#include <common/qt/GenericTableModel.hpp>
#include <common/qt/LapDataProvider.hpp>
#include <common/qt/LapListModel.hpp>
#include <kdbindings/property.h>

namespace Rapid::Workflow::Qt
{

/**
 * Alias for a LapModel
 */
using LapModel = Rapid::Common::Qt::GenericTableModel<Rapid::Common::Qt::LapDataProvider>;

/**
 * Alias for a LapListModel
 */
using LapListModel = Rapid::Common::Qt::LapListModel;

class ISessionAnalyzeWorkflow
{
public:
    /**
     * Default destructor
     */
    virtual ~ISessionAnalyzeWorkflow() = default;

    /**
     * Disabled copy constructor
     */
    ISessionAnalyzeWorkflow(ISessionAnalyzeWorkflow const&) = delete;

    /**
     * Disablbed copy assignment operator
     */
    ISessionAnalyzeWorkflow& operator=(ISessionAnalyzeWorkflow const&) = delete;

    /**
     * Disabled move constructor
     */
    ISessionAnalyzeWorkflow(ISessionAnalyzeWorkflow&&) noexcept = delete;

    /**
     * Disablbed move assignment operator
     */
    ISessionAnalyzeWorkflow& operator=(ISessionAnalyzeWorkflow&&) noexcept = delete;

    /**
     * Updates the session for analyzing.
     * When a new session is set the all properties of the @ref ISessionAnalyzeWorkflow are reevalulated.
     * @param session The new session which shall be analyzed.
     */
    virtual void setSession(Common::SessionData const& session) = 0;

    /**
     * Gives a model with all laps of the session set with @ref ISessionAnalyzerWorkflow::setSession
     * The model can be used to display laps of the session in QTableView
     */
    KDBindings::Property<std::shared_ptr<LapModel>> lapModel;

    /**
     * @brief Gives a list model for display laps in a list
     *
     * @details The primary use of a lap list model is for displaying the laps of the session in QML.
     */
    KDBindings::Property<std::shared_ptr<LapListModel>> lapListModel;

protected:
    /**
     * Default constructor
     */
    ISessionAnalyzeWorkflow() = default;
};

} // namespace Rapid::Workflow::Qt

#endif // ISESSIONANALYZEWORKFLOW_HPP
