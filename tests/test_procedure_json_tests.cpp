#include <iostream>
#include <string>

#include "test_procedure_json.h"

int main()
{
   
    const std::string valid_json = R"(
{
    "name": "Relay smoke test",
    "steps": [
        {
            "name": "Switch relay on",
            "action": "relay_on"
        },
        {
            "name": "Verify relay on",
            "action": "expect_relay_on",
            "retries": 2,
            "timeout_ms": 500
        },
        {
            "name": "Switch relay off",
            "action": "relay_off"
        },
        {
            "name": "Verify relay off",
            "action": "expect_relay_off"
        }
    ]
}
)";

    TestProcedure procedure;
    std::string error_message;

    const bool parsed = parse_test_procedure_json(
        valid_json,
        procedure,
        error_message);

    if (parsed == false)
    {
        std::cerr << "FAIL: valid JSON should be accepted\n";
        std::cerr << "Error: " << error_message << '\n';
        return 1;
    }

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
        std::cerr << "FAIL: first action is incorrect\n";
        return 1;
    }

    if (procedure.steps[1].action != TestAction::expect_relay_on)
    {
        std::cerr << "FAIL: second action is incorrect\n";
        return 1;
    }

    if (procedure.steps[2].action != TestAction::relay_off)
    {
        std::cerr << "FAIL: third action is incorrect\n";
        return 1;
    }

    if (procedure.steps[3].action != TestAction::expect_relay_off)
    {
        std::cerr << "FAIL: fourth action is incorrect\n";
        return 1;
    }

    if (procedure.steps[0].retries != 0)
    {
        std::cerr
            << "FAIL: missing retries should default to zero\n";
        return 1;
    }

    if (procedure.steps[1].retries != 2)
    {
        std::cerr << "FAIL: retry count was not parsed\n";
        return 1;
    }

    if (procedure.steps[0].timeout_ms != 1000)
    {
        std::cerr
            << "FAIL: missing timeout should default to 1000 ms\n";
        return 1;
    }

    if (procedure.steps[1].timeout_ms != 500)
    {
        std::cerr << "FAIL: timeout was not parsed\n";
        return 1;
    }

    const std::string unknown_action_json = R"(
    {
        "name": "Invalid action test",
        "steps": [
            {
                "name": "Invalid action",
                "action": "explode_relay"
            }
        ]
    }
    )";

    TestProcedure unknown_action_procedure;
    std::string unknown_action_error;

    const bool unknown_action_parsed =
        parse_test_procedure_json(
            unknown_action_json,
            unknown_action_procedure,
            unknown_action_error);

    if (unknown_action_parsed)
    {
        std::cerr << "FAIL: unknown action should be rejected\n";
        return 1;
    }

    if (unknown_action_error !=
        "unknown test action: explode_relay")
    {
        std::cerr << "FAIL: unknown action error is incorrect\n";
        std::cerr << "Actual error: "
                  << unknown_action_error << '\n';
        return 1;
    }
    
    const std::string invalid_retries_json = R"(
    {
        "name": "Invalid retries",
        "steps": [
            {
                "name": "Switch relay on",
                "action": "relay_on",
                "retries": -1
            }
        ]
    }
    )";

    TestProcedure invalid_retries_procedure;
    std::string invalid_retries_error;

    const bool invalid_retries_parsed =
        parse_test_procedure_json(
            invalid_retries_json,
            invalid_retries_procedure,
            invalid_retries_error);

    if (invalid_retries_parsed)
    {
        std::cerr << "FAIL: negative retries should be rejected\n";
        return 1;
    }

    if (invalid_retries_error !=
        "step retries must be a non-negative integer")
    {
        std::cerr << "FAIL: invalid retries error is incorrect\n";
        std::cerr << "Actual error: "
                << invalid_retries_error << '\n';
        return 1;
    }
    const std::string invalid_timeout_json = R"(
    {
        "name": "Invalid timeout",
        "steps": [
            {
                "name": "Switch relay on",
                "action": "relay_on",
                "timeout_ms": 0
            }
        ]
    }
    )";

    TestProcedure invalid_timeout_procedure;
    std::string invalid_timeout_error;

    const bool invalid_timeout_parsed =
        parse_test_procedure_json(
            invalid_timeout_json,
            invalid_timeout_procedure,
            invalid_timeout_error);

    if (invalid_timeout_parsed)
    {
        std::cerr << "FAIL: zero timeout should be rejected\n";
        return 1;
    }

    if (invalid_timeout_error !=
        "step timeout_ms must be a positive integer")
    {
        std::cerr << "FAIL: invalid timeout error is incorrect\n";
        std::cerr << "Actual error: "
                << invalid_timeout_error << '\n';
        return 1;
    }

    std::cout << "PASS: JSON procedure parsing tests\n";
    return 0;
}

