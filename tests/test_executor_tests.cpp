#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>

#include "simulated_relay.h"
#include "test_executor.h"

class RecoveringRelay : public IRelay
{
public:
    void turn_on() override
    {
        is_on_ = true;
    }

    void turn_off() override
    {
        is_on_ = false;
    }

    bool is_on() const override
    {
        if (remaining_failed_reads_ > 0)
        {
            --remaining_failed_reads_;
            return false;
        }

        return is_on_;
    }

private:
    bool is_on_{true};
    mutable int remaining_failed_reads_{1};
};

class SlowRelay : public IRelay
{
public:
    void turn_on() override
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(20));

        is_on_ = true;
    }

    void turn_off() override
    {
        is_on_ = false;
    }

    bool is_on() const override
    {
        return is_on_;
    }

private:
    bool is_on_{false};
};

int main()
{
    TestProcedure passing_procedure{
        "Relay smoke test",
        {
            {"Switch relay on", TestAction::relay_on},
            {"Verify relay on", TestAction::expect_relay_on},
            {"Switch relay off", TestAction::relay_off},
            {"Verify relay off", TestAction::expect_relay_off}
        }
    };

    SimulatedRelay relay;
    std::ostringstream output;

    TestResult result =
        execute_procedure(passing_procedure, relay, output);

    if (result.passed == false)
    {
        std::cerr << "FAIL: valid relay procedure should pass\n";
        return 1;
    }

    if (result.completed_steps != 4)
    {
        std::cerr << "FAIL: four steps should be completed\n";
        return 1;
    }

    if (result.failed_step.has_value())
    {
        std::cerr << "FAIL: passing procedure should not have a failed step\n";
        return 1;
    }

    if (result.message != "All steps passed")
    {
        std::cerr << "FAIL: passing result message is incorrect\n";
        return 1;
    }

    if (relay.is_on())
    {
        std::cerr << "FAIL: relay should finish in the off state\n";
        return 1;
    }

    const std::string expected_output =
        "Running procedure: Relay smoke test\n"
        "Step: Switch relay on\n"
        "Step: Verify relay on\n"
        "Step: Switch relay off\n"
        "Step: Verify relay off\n"
        "Result: PASS\n";

    if (output.str() != expected_output)
    {
        std::cerr << "FAIL: procedure output is incorrect\n";
        return 1;
    }

    TestProcedure failing_procedure{
        "Invalid relay state test",
        {
            {"Verify relay on", TestAction::expect_relay_on}
        }
    };

    SimulatedRelay failing_relay;
    std::ostringstream failure_output;

    TestResult failure_result =
        execute_procedure(failing_procedure, failing_relay, failure_output);

    if (failure_result.passed)
    {
        std::cerr << "FAIL: invalid relay state should fail\n";
        return 1;
    }

    if (failure_result.completed_steps != 0)
    {
        std::cerr << "FAIL: failed step should not be counted as completed\n";
        return 1;
    }

    if (failure_result.failed_step.has_value() == false)
    {
        std::cerr << "FAIL: failure should identify the failed step\n";
        return 1;
    }

    if (failure_result.failed_step.value() != "Verify relay on")
    {
        std::cerr << "FAIL: failed step name is incorrect\n";
        return 1;
    }

    if (failure_result.message != "Expected relay to be on")
    {
        std::cerr << "FAIL: failure message is incorrect\n";
        return 1;
    }

    const std::string expected_failure_output =
        "Running procedure: Invalid relay state test\n"
        "Step: Verify relay on\n"
        "Result: FAIL\n";

    if (failure_output.str() != expected_failure_output)
    {
        std::cerr << "FAIL: failure output is incorrect\n";
        return 1;
    }
    TestProcedure retry_failure_procedure{
        "Retry failure test",
        {
            {
                "Verify relay on",
                TestAction::expect_relay_on,
                2
            }
        }
    };

    SimulatedRelay retry_failure_relay;
    std::ostringstream retry_failure_output;

    const TestResult retry_failure_result =
        execute_procedure(
            retry_failure_procedure,
            retry_failure_relay,
            retry_failure_output);

    if (retry_failure_result.passed)
    {
        std::cerr
            << "FAIL: exhausted retries should fail\n";
        return 1;
    }

    if (retry_failure_result.completed_steps != 0)
    {
        std::cerr
            << "FAIL: exhausted step should not be completed\n";
        return 1;
    }

    const std::string expected_retry_failure_output =
        "Running procedure: Retry failure test\n"
        "Step: Verify relay on\n"
        "Retrying step: Verify relay on (attempt 2 of 3)\n"
        "Retrying step: Verify relay on (attempt 3 of 3)\n"
        "Result: FAIL\n";

    if (retry_failure_output.str() !=
        expected_retry_failure_output)
    {
        std::cerr << "FAIL: retry output is incorrect\n";
        std::cerr << "Expected:\n"
            << expected_retry_failure_output;
        std::cerr << "Actual:\n"
            << retry_failure_output.str();
        return 1;
    }

    TestProcedure retry_success_procedure{
        "Retry recovery test",
        {
            {
                "Verify relay on",
                TestAction::expect_relay_on,
                1
            }
        }
    };

    RecoveringRelay recovering_relay;
    std::ostringstream retry_success_output;

    const TestResult retry_success_result =
        execute_procedure(
            retry_success_procedure,
            recovering_relay,
            retry_success_output);

    if (retry_success_result.passed == false)
    {
        std::cerr
            << "FAIL: second attempt should succeed\n";
        return 1;
    }

    if (retry_success_result.completed_steps != 1)
    {
        std::cerr
            << "FAIL: recovered step should be completed\n";
        return 1;
    }

    if (retry_success_result.failed_step.has_value())
    {
        std::cerr
            << "FAIL: recovered procedure should not have a failed step\n";
        return 1;
    }

    const std::string expected_retry_success_output =
        "Running procedure: Retry recovery test\n"
        "Step: Verify relay on\n"
        "Retrying step: Verify relay on (attempt 2 of 2)\n"
        "Result: PASS\n";

    if (retry_success_output.str() !=
        expected_retry_success_output)
    {
        std::cerr << "FAIL: retry recovery output is incorrect\n";
        std::cerr << "Expected:\n"
                << expected_retry_success_output;
        std::cerr << "Actual:\n"
                << retry_success_output.str();
        return 1;
    }
    
    TestProcedure timeout_procedure{
        "Timeout test",
        {
            {
                "Slow relay activation",
                TestAction::relay_on,
                0,
                1
            }
        }
    };

    SlowRelay slow_relay;
    std::ostringstream timeout_output;

    const TestResult timeout_result =
        execute_procedure(
            timeout_procedure,
            slow_relay,
            timeout_output);

    if (timeout_result.passed)
    {
        std::cerr << "FAIL: slow step should time out\n";
        return 1;
    }

    if (timeout_result.completed_steps != 0)
    {
        std::cerr
            << "FAIL: timed-out step should not be completed\n";
        return 1;
    }

    if (timeout_result.failed_step.has_value() == false)
    {
        std::cerr
            << "FAIL: timeout should identify the failed step\n";
        return 1;
    }

    if (timeout_result.failed_step.value() !=
        "Slow relay activation")
    {
        std::cerr << "FAIL: timed-out step name is incorrect\n";
        return 1;
    }

    if (timeout_result.message !=
        "Step exceeded timeout of 1 ms")
    {
        std::cerr << "FAIL: timeout message is incorrect\n";
        std::cerr << "Actual message: "
                << timeout_result.message << '\n';
        return 1;
    }

    const std::string expected_timeout_output =
        "Running procedure: Timeout test\n"
        "Step: Slow relay activation\n"
        "Result: FAIL\n";

    if (timeout_output.str() != expected_timeout_output)
    {
        std::cerr << "FAIL: timeout output is incorrect\n";
        return 1;
    }

    TestProcedure cancellation_procedure{
        "Cancellation test",
        {
            {"Switch relay on", TestAction::relay_on},
            {"Switch relay off", TestAction::relay_off}
        }
    };

    SimulatedRelay cancellation_relay;
    std::ostringstream cancellation_output;
    std::atomic_bool cancellation_requested{true};

    const TestResult cancellation_result =
        execute_procedure(
            cancellation_procedure,
            cancellation_relay,
            cancellation_output,
            &cancellation_requested);

    if (cancellation_result.cancelled == false)
    {
        std::cerr
            << "FAIL: cancellation should be reported\n";
        return 1;
    }

    if (cancellation_result.passed)
    {
        std::cerr
            << "FAIL: cancelled procedure should not pass\n";
        return 1;
    }

    if (cancellation_result.completed_steps != 0)
    {
        std::cerr
            << "FAIL: cancelled procedure should complete no steps\n";
        return 1;
    }

    if (cancellation_result.failed_step.has_value())
    {
        std::cerr
            << "FAIL: cancellation before execution has no failed step\n";
        return 1;
    }

    if (cancellation_result.message != "Procedure cancelled")
    {
        std::cerr << "FAIL: cancellation message is incorrect\n";
        return 1;
    }

    if (cancellation_relay.is_on())
    {
        std::cerr
            << "FAIL: cancelled procedure should not modify relay\n";
        return 1;
    }

    const std::string expected_cancellation_output =
        "Running procedure: Cancellation test\n"
        "Result: CANCELLED\n";

    if (cancellation_output.str() !=
        expected_cancellation_output)
    {
        std::cerr << "FAIL: cancellation output is incorrect\n";
        return 1;
    }

    std::cout << "PASS: structured test results\n";
    return 0;
}
