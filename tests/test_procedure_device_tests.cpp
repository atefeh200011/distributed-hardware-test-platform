#include <iostream>
#include <string>

#include "test_procedure_json.h"

int main()
{
    const std::string valid_json = R"(
{
    "name": "Multiple relay test",
    "steps": [
        {
            "name": "Switch first relay on",
            "device": "relay-1",
            "action": "relay_on"
        },
        {
            "name": "Switch second relay on",
            "device": "relay-2",
            "action": "relay_on"
        },
        {
            "name": "Default relay status",
            "action": "expect_relay_on"
        }
    ]
}
)";

    TestProcedure procedure;
    std::string error_message;

    if (parse_test_procedure_json(
            valid_json,
            procedure,
            error_message) == false)
    {
        std::cerr
            << "FAIL: valid device JSON was rejected\n";
        std::cerr
            << "Error: "
            << error_message
            << '\n';
        return 1;
    }

    if (procedure.steps.size() != 3)
    {
        std::cerr
            << "FAIL: expected three procedure steps\n";
        return 1;
    }

    if (procedure.steps[0].device != "relay-1")
    {
        std::cerr
            << "FAIL: first device is incorrect\n";
        return 1;
    }

    if (procedure.steps[1].device != "relay-2")
    {
        std::cerr
            << "FAIL: second device is incorrect\n";
        return 1;
    }

    if (procedure.steps[2].device != "relay-1")
    {
        std::cerr
            << "FAIL: missing device should default "
            << "to relay-1\n";
        return 1;
    }

    const std::string empty_device_json = R"(
{
    "name": "Invalid device",
    "steps": [
        {
            "name": "Invalid step",
            "device": "",
            "action": "relay_on"
        }
    ]
}
)";

    TestProcedure invalid_procedure;
    std::string invalid_error;

    if (parse_test_procedure_json(
            empty_device_json,
            invalid_procedure,
            invalid_error))
    {
        std::cerr
            << "FAIL: empty device should be rejected\n";
        return 1;
    }

    if (invalid_error !=
        "step device must not be empty")
    {
        std::cerr
            << "FAIL: empty-device error is incorrect\n";
        std::cerr
            << "Actual: "
            << invalid_error
            << '\n';
        return 1;
    }

    const std::string wrong_type_json = R"(
{
    "name": "Invalid device type",
    "steps": [
        {
            "name": "Invalid step",
            "device": 42,
            "action": "relay_on"
        }
    ]
}
)";

    invalid_error.clear();

    if (parse_test_procedure_json(
            wrong_type_json,
            invalid_procedure,
            invalid_error))
    {
        std::cerr
            << "FAIL: numeric device should be rejected\n";
        return 1;
    }

    if (invalid_error !=
        "step device must be a string")
    {
        std::cerr
            << "FAIL: device-type error is incorrect\n";
        std::cerr
            << "Actual: "
            << invalid_error
            << '\n';
        return 1;
    }

    std::cout
        << "PASS: procedure device parsing tests\n";

    return 0;
}
