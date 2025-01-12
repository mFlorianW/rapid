// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionAnalyzeWorkflow.hpp"
#include <spdlog/spdlog.h>

namespace Rapid::Workflow::Qt
{

SessionAnalyzeWorkflow::SessionAnalyzeWorkflow()
{
    setLapModel();
}

SessionAnalyzeWorkflow::~SessionAnalyzeWorkflow() = default;

void SessionAnalyzeWorkflow::setSession(Common::SessionData const& session) noexcept
{
    if (mSession != session) {
        mSession = session;
        setLapModel();
    }
}

void SessionAnalyzeWorkflow::setLapModel() noexcept
{
    try {
        mLapDataProvider = std::make_unique<Common::Qt::LapDataProvider>(mSession.getTrack(), mSession.getLaps());
        lapModel.set(std::make_shared<LapModel>(*mLapDataProvider));
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Failed create lap model not sufficient memory. Error: {}", e.what());
    }
}

} // namespace Rapid::Workflow::Qt
