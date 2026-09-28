#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "relay_registry.h"
#include "simulated_relay.h"
#include "test_executor.h"

int main()
{
    RelayRegistry relays;

    const auto relay_1 =
        std::make_shared<SimulatedRelay>();

    const auto relay_2 =
        std::make_shared<SimulatedRelay>();

    if (relays.add("relay-1", relay_1) == false ||
        relays.add("relay-2", relay_2) == false)
    {
        std::cerr
            << "FAIL: relays should be registered\n";
        return 1;
    }

    const TestProcedure procedure{
        "Multiple relay test",
        {
            {
                "Switch first relay on",
                TestAction::relay_on,
                0,
                1000,
                "relay-1"
            },
            {
                "Verify first relay on",
                TestAction::expect_relay_on,
                0,
                1000,
                "relay-1"
            },
            {
                "Switch second relay on",
                TestAction::relay_on,
                0,
                1000,
                "relay-2"
            },
            {
                "Verify second relay on",
                TestAction::expect_relay_on,
                0,
                1000,
                "relay-2"
            },
            {
                "Switch first relay off",
                TestAction::relay_off,
                0,
                1000,
                "relay-1"
            },
            {
                "Verify second relay remains on",
                TestAction::expect_relay_on,
                0,
                1000,
                "relay-2"
            }
        }
    };

    std::ostringstream output;

    const TestResult result =
        execute_procedure(
            procedure,
            relays,
            output);

    if (result.passed == false)
    {
        std::cerr
            << "FAIL: multi-relay procedure should pass\n";
        std::cerr
            << "Message: "
            << result.message
            << '\n';
        return 1;
    }

    if (result.completed_steps != 6)
    {
        std::cerr
            << "FAIL: six steps should be completed\n";
        return 1;
    }

    if (relay_1->is_on())
    {
        std::cerr
            << "FAIL: relay-1 should finish off\n";
        return 1;
    }

    if (relay_2->is_on() == false)
    {
        std::cerr
            << "FAIL: relay-2 should finish on\n";
        return 1;
    }

    const TestProcedure missing_device_procedure{
        "Missing relay test",
        {
            {
                "Use missing relay",
                TestAction::relay_on,
                2,
                1000,
                "missing"
            }
        }
    };

    std::ostringstream missing_output;

    const TestResult missing_result =
        execute_procedure(
            missing_device_procedure,
            relays,
            missing_output);

    if (missing_result.passed)
    {
        std::cerr
            << "FAIL: missing relay should fail\n";
        return 1;
    }

    if (missing_result.completed_steps != 0)
    {
        std::cerr
            << "FAIL: missing relay step "
            << "should not be completed\n";
        return 1;
    }

    if (missing_result.failed_step !=
        "Use missing relay")
    {
        std::cerr
            << "FAIL: missing relay failed-step "
            << "name is incorrect\n";
        return 1;
    }

    if (missing_result.message !=
        "Relay not found: missing")
    {
        std::cerr
            << "FAIL: missing relay message is incorrect\n";
        std::cerr
            << "Actual: "
            << missing_result.message
            << '\n';
        return 1;
    }

    if (missing_result.steps.size() != 1)
    {
        std::cerr
            << "FAIL: missing relay should create "
            << "one step result\n";
        return 1;
    }

    if (missing_result.steps[0].attempts != 0)
    {
        std::cerr
            << "FAIL: missing relay should use "
            << "zero attempts\n";
        return 1;
    }

    const std::string expected_missing_output =
        "Running procedure: Missing relay test\n"
        "Step: Use missing relay\n"
        "Result: FAIL\n";

    if (missing_output.str() !=
        expected_missing_output)
    {
        std::cerr
            << "FAIL: missing relay output is incorrect\n";
        std::cerr
            << "Expected:\n"
            << expected_missing_output;
        std::cerr
            << "Actual:\n"
            << missing_output.str();
        return 1;
    }

    std::cout
        << "PASS: registry executor tests\n";

    return 0;
}
