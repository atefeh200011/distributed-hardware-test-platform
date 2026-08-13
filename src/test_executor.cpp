#include "test_executor.h"

bool execute_procedure(
    const TestProcedure& procedure,
    IRelay& relay,
    std::ostream& output)
{
    output << "Running procedure: " << procedure.name << '\n';

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
                return false;
            }
        }
        else if (step.action == TestAction::expect_relay_off)
        {
            if (relay.is_on() == true)
            {
                output << "Result: FAIL\n";
                return false;
            }
        }
    }

    output << "Result: PASS\n";
    return true;
}
