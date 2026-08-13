#include "test_executor.h"

#include <chrono>
#include <string>

namespace
{
bool execute_step(
    const TestStep& step,
    IRelay& relay,
    std::string& failure_message)
{
    if (step.action == TestAction::relay_on)
    {
        relay.turn_on();
        return true;
    }

    if (step.action == TestAction::relay_off)
    {
        relay.turn_off();
        return true;
    }

    if (step.action == TestAction::expect_relay_on)
    {
        if (relay.is_on() == false)
        {
            failure_message = "Expected relay to be on";
            return false;
        }

        return true;
    }

    if (step.action == TestAction::expect_relay_off)
    {
        if (relay.is_on())
        {
            failure_message = "Expected relay to be off";
            return false;
        }

        return true;
    }

    failure_message = "Unsupported test action";
    return false;
}
}

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

        const std::size_t maximum_attempts =
            step.retries + 1;

        bool step_passed = false;
        std::string failure_message;

        for (std::size_t attempt = 1;
             attempt <= maximum_attempts;
             ++attempt)
        {
            failure_message.clear();

            const auto start_time =
                std::chrono::steady_clock::now();

            const bool attempt_passed =
                execute_step(step, relay, failure_message);

            const auto end_time =
                std::chrono::steady_clock::now();

            const auto elapsed_time =
                std::chrono::duration_cast<
                    std::chrono::milliseconds>(
                    end_time - start_time);

            const bool timed_out =
                elapsed_time >
                std::chrono::milliseconds(step.timeout_ms);

            if (timed_out)
            {
                failure_message =
                    "Step exceeded timeout of " +
                    std::to_string(step.timeout_ms) +
                    " ms";
            }
            else if (attempt_passed)
            {
                step_passed = true;
                break;
            }

            if (attempt < maximum_attempts)
            {
                output
                    << "Retrying step: "
                    << step.name
                    << " (attempt "
                    << (attempt + 1)
                    << " of "
                    << maximum_attempts
                    << ")\n";
            }
        }

        if (step_passed == false)
        {
            output << "Result: FAIL\n";

            return TestResult{
                false,
                completed_steps,
                step.name,
                failure_message
            };
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
