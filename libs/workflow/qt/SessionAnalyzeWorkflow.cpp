// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "SessionAnalyzeWorkflow.hpp"

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

void SessionAnalyzeWorkflow::setLapModel()
{
    mLapDataProvider = std::make_unique<Common::Qt::LapDataProvider>(mSession.getTrack(), mSession.getLaps());
    lapModel.set(std::make_shared<LapModel>(*mLapDataProvider));
}

} // namespace Rapid::Workflow::Qt
