#include <iostream>

#include "test_procedure.h"

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

    if (procedure.name != "Relay smoke test")
    {
        std::cerr << "FAIL: procedure name is incorrect\n";
        return 1;
    }

    if (procedure.steps.size() != 4)
    {
        std::cerr << "FAIL: procedure should contain four steps\n";
        return 1;
    }

    if (procedure.steps[0].action != TestAction::relay_on)
    {
        std::cerr << "FAIL: first action should switch the relay on\n";
        return 1;
    }

    if (procedure.steps[3].action != TestAction::expect_relay_off)
    {
        std::cerr << "FAIL: fourth action should verify the relay is off\n";
        return 1;
    }

    std::cout << "PASS: test procedure model\n";
    return 0;
}
