#include "test_executor.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace
{
using RelayResolver =
    std::function<IRelay*(const TestStep&)>;

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
            failure_message =
                "Expected relay to be on";
            return false;
        }

        return true;
    }

    if (step.action ==
        TestAction::expect_relay_off)
    {
        if (relay.is_on())
        {
            failure_message =
                "Expected relay to be off";
            return false;
        }

        return true;
    }

    failure_message =
        "Unsupported test action";
    return false;
}

std::int64_t elapsed_milliseconds(
    std::chrono::steady_clock::time_point start_time)
{
    const auto elapsed =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
            std::chrono::steady_clock::now() -
            start_time);

    return elapsed.count();
}

TestResult execute_procedure_with_resolver(
    const TestProcedure& procedure,
    const RelayResolver& resolve_relay,
    std::ostream& output,
    const std::atomic_bool* cancellation_requested)
{
    const auto procedure_start_time =
        std::chrono::steady_clock::now();

    output
        << "Running procedure: "
        << procedure.name
        << '\n';

    std::size_t completed_steps = 0;
    std::vector<StepResult> step_results;

    for (const TestStep& step : procedure.steps)
    {
        if (cancellation_requested != nullptr &&
            cancellation_requested->load())
        {
            output << "Result: CANCELLED\n";

            return TestResult{
                false,
                completed_steps,
                std::nullopt,
                "Procedure cancelled",
                true,
                procedure.name,
                elapsed_milliseconds(
                    procedure_start_time),
                step_results
            };
        }

        output
            << "Step: "
            << step.name
            << '\n';

        const auto step_start_time =
            std::chrono::steady_clock::now();

        IRelay* relay =
            resolve_relay(step);

        if (relay == nullptr)
        {
            const std::string failure_message =
                "Relay not found: " +
                step.device;

            step_results.push_back(
                StepResult{
                    step.name,
                    false,
                    0,
                    elapsed_milliseconds(
                        step_start_time),
                    failure_message
                });

            output << "Result: FAIL\n";

            return TestResult{
                false,
                completed_steps,
                step.name,
                failure_message,
                false,
                procedure.name,
                elapsed_milliseconds(
                    procedure_start_time),
                step_results
            };
        }

        const std::size_t maximum_attempts =
            step.retries + 1;

        std::size_t attempts_used = 0;
        bool step_passed = false;
        std::string failure_message;

        for (std::size_t attempt = 1;
             attempt <= maximum_attempts;
             ++attempt)
        {
            if (cancellation_requested != nullptr &&
                cancellation_requested->load())
            {
                const std::string cancellation_message =
                    "Procedure cancelled";

                step_results.push_back(
                    StepResult{
                        step.name,
                        false,
                        attempts_used,
                        elapsed_milliseconds(
                            step_start_time),
                        cancellation_message
                    });

                output << "Result: CANCELLED\n";

                return TestResult{
                    false,
                    completed_steps,
                    step.name,
                    cancellation_message,
                    true,
                    procedure.name,
                    elapsed_milliseconds(
                        procedure_start_time),
                    step_results
                };
            }

            ++attempts_used;
            failure_message.clear();

            const auto attempt_start_time =
                std::chrono::steady_clock::now();

            const bool attempt_passed =
                execute_step(
                    step,
                    *relay,
                    failure_message);

            const auto attempt_end_time =
                std::chrono::steady_clock::now();

            if (cancellation_requested != nullptr &&
                cancellation_requested->load())
            {
                const std::string cancellation_message =
                    "Procedure cancelled";

                step_results.push_back(
                    StepResult{
                        step.name,
                        false,
                        attempts_used,
                        elapsed_milliseconds(
                            step_start_time),
                        cancellation_message
                    });

                output << "Result: CANCELLED\n";

                return TestResult{
                    false,
                    completed_steps,
                    step.name,
                    cancellation_message,
                    true,
                    procedure.name,
                    elapsed_milliseconds(
                        procedure_start_time),
                    step_results
                };
            }

            const auto attempt_duration =
                std::chrono::duration_cast<
                    std::chrono::milliseconds>(
                    attempt_end_time -
                    attempt_start_time);

            const bool timed_out =
                attempt_duration >
                std::chrono::milliseconds(
                    step.timeout_ms);

            if (timed_out)
            {
                failure_message =
                    "Step exceeded timeout of " +
                    std::to_string(
                        step.timeout_ms) +
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

        const std::int64_t step_duration_ms =
            elapsed_milliseconds(
                step_start_time);

        if (step_passed == false)
        {
            step_results.push_back(
                StepResult{
                    step.name,
                    false,
                    attempts_used,
                    step_duration_ms,
                    failure_message
                });

            output << "Result: FAIL\n";

            return TestResult{
                false,
                completed_steps,
                step.name,
                failure_message,
                false,
                procedure.name,
                elapsed_milliseconds(
                    procedure_start_time),
                step_results
            };
        }

        step_results.push_back(
            StepResult{
                step.name,
                true,
                attempts_used,
                step_duration_ms,
                "Step passed"
            });

        ++completed_steps;
    }

    output << "Result: PASS\n";

    return TestResult{
        true,
        completed_steps,
        std::nullopt,
        "All steps passed",
        false,
        procedure.name,
        elapsed_milliseconds(
            procedure_start_time),
        step_results
    };
}
}

TestResult execute_procedure(
    const TestProcedure& procedure,
    IRelay& relay,
    std::ostream& output,
    const std::atomic_bool* cancellation_requested)
{
    const RelayResolver resolver =
        [&relay](const TestStep&)
        {
            return &relay;
        };

    return execute_procedure_with_resolver(
        procedure,
        resolver,
        output,
        cancellation_requested);
}

TestResult execute_procedure(
    const TestProcedure& procedure,
    RelayRegistry& relays,
    std::ostream& output,
    const std::atomic_bool* cancellation_requested)
{
    const RelayResolver resolver =
        [&relays](const TestStep& step)
        {
            return relays.find(step.device);
        };

    return execute_procedure_with_resolver(
        procedure,
        resolver,
        output,
        cancellation_requested);
}
