// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rest/RestCall.hpp"
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>

using namespace Rapid::Rest;

class TestableRestCall : public RestCall
{
public:
    void setCallResult(RestCallResult result) noexcept override
    {
        try {
            mResult = result;
            finished.emit(this);
        } catch (std::exception const& e) {
            SPDLOG_ERROR("Failed ot emit finished signal. Error already emitting.");
        }
    }
};

SCENARIO("A RestCall shall return the result of the call, emit the finished signal and give the call payload")
{
    GIVEN("A RestCall")
    {
        auto call = TestableRestCall{};
        auto returnData = std::string{"Test"};
        auto finishedSignalEmitted = false;

        WHEN("The data and the result are set.")
        {
            REQUIRE(call.getResult() == RestCallResult::Unknown);
            std::ignore = call.finished.connect([&](RestCall*) {
                finishedSignalEmitted = true;
            });
            call.setData(returnData);
            call.setCallResult(RestCallResult::Success);

            THEN("The correct result, data and signal shall be emitted")
            {
                REQUIRE(finishedSignalEmitted == true);
                REQUIRE(call.isFinished() == true);
                REQUIRE(call.getResult() == RestCallResult::Success);
                REQUIRE(call.getData() == returnData);
            }
        }
    }
}
