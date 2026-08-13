#include "test_executor.h"

TestResult execute_procedure(
    const TestProcedure& procedure,
    IRelay& relay,
    std::ostream& output)
{
    output << "Running procedure: " << procedure.name << '\n';

    std::size_t completed_steps = 0;

    for (const TestStep& step : procedure.steps)
    {
        output << "Step: " << step.name << '\n';

        if (step.action == TestAction::relay_on)
        {
            relay.turn_on();
        }
        else if (step.action == TestAction::relay_off)
        {
            relay.turn_off();
        }
        else if (step.action == TestAction::expect_relay_on)
        {
            if (relay.is_on() == false)
            {
                output << "Result: FAIL\n";

                return TestResult{
                    false,
                    completed_steps,
                    step.name,
                    "Expected relay to be on"
                };
            }
        }
        else if (step.action == TestAction::expect_relay_off)
        {
            if (relay.is_on() == true)
            {
                output << "Result: FAIL\n";

                return TestResult{
                    false,
                    completed_steps,
                    step.name,
                    "Expected relay to be off"
                };
            }
        }

        ++completed_steps;
    }

    output << "Result: PASS\n";

    return TestResult{
        true,
        completed_steps,
        std::nullopt,
        "All steps passed"
    };
}