// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "system/AsyncResult.hpp"
#include <catch2/catch_all.hpp>
#include <thread>

using namespace Rapid::System;

namespace
{
class TestAsyncResult : public AsyncResult
{
public:
    using AsyncResult::AsyncResult;

    void setResult(Result result, std::string const& errorMessage = std::string{})
    {
        AsyncResult::setResult(result, errorMessage);
    }
};

class TestAsyncResultWithValue : public AsyncResultWithValue<int>
{
public:
    using AsyncResultWithValue::AsyncResultWithValue;

    void setResult(Result result, std::string const& errorMessage = std::string{})
    {
        AsyncResult::setResult(result, errorMessage);
    }
};

std::shared_ptr<AsyncResult> startLongOperation(std::thread& thread)
{
    auto result = std::make_shared<TestAsyncResult>();
    thread = std::thread([result]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        result->setResult(Result::Ok);
    });
    return result;
}

std::shared_ptr<AsyncResultWithValue<int>> startLongOperationWithValue(std::thread& thread)
{
    auto result = std::make_shared<TestAsyncResultWithValue>();
    thread = std::thread([result]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        result->setResultValue(123);
        result->setResult(Result::Ok);
    });
    return result;
}

std::shared_ptr<AsyncResultWithValue<int>> startLongOperationWithValueError(std::thread& thread)
{
    auto result = std::make_shared<TestAsyncResultWithValue>();
    thread = std::thread([result]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        result->setResult(Result::Error, std::string{"Error occured!"});
    });
    return result;
}

} // namespace

SCENARIO("The AsyncResult shall emit done signal when operation is finshed")
{
    GIVEN("A AsyncResult")
    {
        auto doneSignalEmitted = false;
        auto aResult = TestAsyncResult();
        auto* receivedResultPointer = static_cast<AsyncResult*>(nullptr);
        std::ignore = aResult.done.connect([&doneSignalEmitted, &receivedResultPointer](AsyncResult* asyncResult) {
            receivedResultPointer = asyncResult;
            doneSignalEmitted = true;
        });

        WHEN("The result is set in the AsyncResult")
        {
            aResult.setResult(Result::Ok);
            THEN("The signal shall be emitted.")
            {
                REQUIRE(doneSignalEmitted == true);
            }

            THEN("The signal parameter shall be the same as the origin result")
            {
                REQUIRE(receivedResultPointer == &aResult);
            }
        }
    }
}

SCENARIO("The AsyncResult shall set the result when operation is finshed")
{
    GIVEN("A AsyncResult")
    {
        auto aResult = TestAsyncResult{};

        WHEN("The result is set in the AsyncResult")
        {
            aResult.setResult(Result::Ok);
            THEN("The correct result type shall be returned")
            {
                REQUIRE(aResult.getResult() == Result::Ok);
            }
        }

        WHEN("No result is set in the AsyncResult")
        {
            THEN("The default return type shall be returned")
            {
                REQUIRE(aResult.getResult() == Result::NotFinished);
            }
        }
    }
}

SCENARIO("The AsyncResult shall return the error message when set")
{
    GIVEN("A AsyncResult")
    {
        auto aResult = TestAsyncResult{};
        auto errMsg = std::string{"This is a errr message."};

        WHEN("The result with a error message is set in the AsyncResult")
        {
            aResult.setResult(Result::Ok, errMsg);
            THEN("The correct result type shall be returned")
            {
                REQUIRE(aResult.getErrorMessage() == errMsg);
            }
        }
    }
}

SCENARIO("The AsyncResult shall be to suspend the current thread and should wait for the result")
{
    GIVEN("A AsyncResult")
    {
        auto thread = std::thread{};

        WHEN("The AsyncResult suspend the current running thread")
        {
            auto aResult = startLongOperation(thread);
            aResult->waitForFinished();
            THEN("The AsyncResult should be there")
            {
                CHECK(aResult->getResult() == Result::Ok);
            }
        }
        thread.join();
    }
}

SCENARIO("The AsyncResultWithValue shall return the value when the operation is finished")
{
    GIVEN("A AsyncResultWithValue")
    {
        auto thread = std::thread{};
        WHEN("The AsyncResultWithValue has a value")
        {
            auto aResult = startLongOperationWithValue(thread);
            aResult->waitForFinished();
            THEN("The AsyncResultWithValue returns the correct value")
            {
                CHECK(aResult->getResult() == Result::Ok);
                CHECK(aResult->getResultValue().has_value());
                REQUIRE(aResult->getResultValue() == 123);
            }
            thread.join();
        }

        WHEN("TestAsyncResultWithValue has no value")
        {
            auto aResult = startLongOperationWithValueError(thread);
            aResult->waitForFinished();
            THEN("The AsyncResultWithValue returns the error code and optional message")
            {
                CHECK_FALSE(aResult->getResultValue().has_value());
                REQUIRE(aResult->getErrorMessage() == std::string_view{"Error occured!"});
            }
            thread.join();
        }
    }
}
