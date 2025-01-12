// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SESSIONANALYZERWORKFLOW_HPP
#define SESSIONANALYZERWORKFLOW_HPP

#include "ISessionAnalyzeWorkflow.hpp"
#include <common/qt/LapDataProvider.hpp>

namespace Rapid::Workflow::Qt
{

class SessionAnalyzeWorkflow : public ISessionAnalyzeWorkflow
{
public:
    /**
     * Creates an instance of the @ref SessionAnalyzerWorkflow
     */
    SessionAnalyzeWorkflow();

    /**
     * Default destructor
     */
    ~SessionAnalyzeWorkflow() override;

    /**
     * Disabled copy constructor
     */
    SessionAnalyzeWorkflow(SessionAnalyzeWorkflow const&) = delete;

    /**
     * Disablbed copy assignment operator
     */
    SessionAnalyzeWorkflow& operator=(SessionAnalyzeWorkflow const&) = delete;

    /**
     * Disabled move constructor
     */
    SessionAnalyzeWorkflow(SessionAnalyzeWorkflow&&) noexcept = delete;

    /**
     * Disablbed move assignment operator
     */
    SessionAnalyzeWorkflow& operator=(SessionAnalyzeWorkflow&&) noexcept = delete;

    /**
     * @copydoc ISessionAnalyzerWorkflow::setSession
     */
    void setSession(Common::SessionData const& session) noexcept override;

private:
    void setLapModel() noexcept;

    Common::SessionData mSession;
    std::unique_ptr<Common::Qt::LapDataProvider> mLapDataProvider;
};

} // namespace Rapid::Workflow::Qt

#endif // SESSIONANALYZERWORKFLOW_HPP
