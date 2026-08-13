#include <iostream>
#include <string>

#include "test_procedure_json.h"

int main()
{
    const std::string valid_json = R"json(
{
    "name": "Relay smoke test",
    "steps": [
        {"name": "Switch relay on", "action": "relay_on"},
        {"name": "Verify relay on", "action": "expect_relay_on"},
        {"name": "Switch relay off", "action": "relay_off"},
        {"name": "Verify relay off", "action": "expect_relay_off"}
    ]
}
)json";

    TestProcedure procedure;
    std::string error_message;

    bool parsed = parse_test_procedure_json(
        valid_json,
        procedure,
        error_message);

    if (parsed == false)
    {
        std::cerr << "FAIL: valid JSON was rejected: "
                  << error_message << '\n';
        return 1;
    }

    if (procedure.name != "Relay smoke test" ||
        procedure.steps.size() != 4)
    {
        std::cerr << "FAIL: parsed procedure is incorrect\n";
        return 1;
    }

    const std::string invalid_json = R"json(
{
    "name": "Invalid test",
    "steps": [
        {"name": "Invalid action", "action": "explode_relay"}
    ]
}
)json";

    parsed = parse_test_procedure_json(
        invalid_json,
        procedure,
        error_message);

    if (parsed == true)
    {
        std::cerr << "FAIL: unknown action was accepted\n";
        return 1;
    }

    if (error_message != "unknown test action: explode_relay")
    {
        std::cerr << "FAIL: unexpected validation message\n";
        return 1;
    }

    std::cout << "PASS: JSON procedure parser\n";
    return 0;
}
