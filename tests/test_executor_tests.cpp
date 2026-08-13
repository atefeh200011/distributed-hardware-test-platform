#include <iostream>
#include <sstream>
#include <string>

#include "simulated_relay.h"
#include "test_executor.h"

int main()
{
    TestProcedure procedure{
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

    bool passed = execute_procedure(procedure, relay, output);

    if (passed == false)
    {
        std::cerr << "FAIL: valid relay procedure should pass\n";
        return 1;
    }

    if (relay.is_on() == true)
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

    std::cout << "PASS: test executor\n";
    return 0;
}
