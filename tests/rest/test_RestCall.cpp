// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestCall.hpp"
#include <catch2/catch_all.hpp>

using namespace Rapid::Rest;

class TestableRestCall : public RestCall
{
public:
    void setCallResult(RestCallResult result) noexcept override
    {
        mResult = result;
        finished.emit(this);
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
            call.finished.connect([&](RestCall*) {
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
