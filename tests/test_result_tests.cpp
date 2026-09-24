#include <iostream>
#include <string>

#include "test_result.h"

int main()
{
    const StepResult step_result{
        "Verify relay on",
        true,
        2,
        15,
        "Step passed"
    };

    const TestResult test_result{
        true,
        1,
        std::nullopt,
        "All steps passed",
        false,
        "Relay smoke test",
        15,
        {step_result}
    };

    if (test_result.procedure_name != "Relay smoke test")
    {
        std::cerr << "FAIL: procedure name is incorrect\n";
        return 1;
    }

    if (test_result.duration_ms != 15)
    {
        std::cerr << "FAIL: procedure duration is incorrect\n";
        return 1;
    }

    if (test_result.steps.size() != 1)
    {
        std::cerr << "FAIL: one step result was expected\n";
        return 1;
    }

    const StepResult& stored_step =
        test_result.steps.front();

    if (stored_step.name != "Verify relay on")
    {
        std::cerr << "FAIL: step name is incorrect\n";
        return 1;
    }

    if (stored_step.passed == false)
    {
        std::cerr << "FAIL: step should have passed\n";
        return 1;
    }

    if (stored_step.attempts != 2)
    {
        std::cerr << "FAIL: attempt count is incorrect\n";
        return 1;
    }

    if (stored_step.duration_ms != 15)
    {
        std::cerr << "FAIL: step duration is incorrect\n";
        return 1;
    }

    if (stored_step.message != "Step passed")
    {
        std::cerr << "FAIL: step message is incorrect\n";
        return 1;
    }

    std::cout << "PASS: detailed test result model\n";
    return 0;
}
