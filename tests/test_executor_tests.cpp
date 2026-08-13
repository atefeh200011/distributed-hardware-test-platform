#include <iostream>
#include <sstream>
#include <string>

#include "simulated_relay.h"
#include "test_executor.h"

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

    std::cout << "PASS: structured test results\n";
    return 0;
}
