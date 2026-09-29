#include <iostream>
#include <string>

#include "device_configuration_json.h"

namespace
{
bool expect_parse_failure(
    const std::string& json_text,
    const std::string& expected_error,
    const std::string& test_name)
{
    PlatformConfiguration configuration;
    std::string error_message;

    if (parse_device_configuration_json(
            json_text,
            configuration,
            error_message))
    {
        std::cerr
            << "FAIL: "
            << test_name
            << " should be rejected\n";
        return false;
    }

    if (error_message != expected_error)
    {
        std::cerr
            << "FAIL: "
            << test_name
            << " produced an incorrect error\n";
        std::cerr
            << "Expected: "
            << expected_error
            << '\n';
        std::cerr
            << "Actual: "
            << error_message
            << '\n';
        return false;
    }

    return true;
}
}

int main()
{
    const std::string valid_json = R"(
{
    "devices": [
        {
            "name": "relay-1",
            "type": "simulated_relay",
            "initial_state": "off"
        },
        {
            "name": "relay-2",
            "type": "simulated_relay",
            "initial_state": "on"
        },
        {
            "name": "relay-3",
            "type": "simulated_relay"
        }
    ]
}
)";

    PlatformConfiguration configuration;
    std::string error_message;

    if (parse_device_configuration_json(
            valid_json,
            configuration,
            error_message) == false)
    {
        std::cerr
            << "FAIL: valid configuration was rejected\n";
        std::cerr
            << "Error: "
            << error_message
            << '\n';
        return 1;
    }

    if (configuration.devices.size() != 3)
    {
        std::cerr
            << "FAIL: expected three devices\n";
        return 1;
    }

    if (configuration.devices[0].name != "relay-1" ||
        configuration.devices[0].initial_state)
    {
        std::cerr
            << "FAIL: relay-1 configuration is incorrect\n";
        return 1;
    }

    if (configuration.devices[1].name != "relay-2" ||
        configuration.devices[1].initial_state == false)
    {
        std::cerr
            << "FAIL: relay-2 configuration is incorrect\n";
        return 1;
    }

    if (configuration.devices[2].initial_state)
    {
        std::cerr
            << "FAIL: omitted initial state should default "
            << "to off\n";
        return 1;
    }

    if (expect_parse_failure(
            R"({"devices":[]})",
            "configuration must contain at least one device",
            "empty device list") == false)
    {
        return 1;
    }

    if (expect_parse_failure(
            R"({
                "devices": [
                    {
                        "name": "",
                        "type": "simulated_relay"
                    }
                ]
            })",
            "device name must not be empty",
            "empty device name") == false)
    {
        return 1;
    }

    if (expect_parse_failure(
            R"({
                "devices": [
                    {
                        "name": "relay-1",
                        "type": "simulated_relay"
                    },
                    {
                        "name": "relay-1",
                        "type": "simulated_relay"
                    }
                ]
            })",
            "duplicate device name: relay-1",
            "duplicate device name") == false)
    {
        return 1;
    }

    if (expect_parse_failure(
            R"({
                "devices": [
                    {
                        "name": "relay-1",
                        "type": "physical_relay"
                    }
                ]
            })",
            "unknown device type: physical_relay",
            "unknown device type") == false)
    {
        return 1;
    }

    if (expect_parse_failure(
            R"({
                "devices": [
                    {
                        "name": "relay-1",
                        "type": "simulated_relay",
                        "initial_state": "invalid"
                    }
                ]
            })",
            "device initial_state must be \"on\" or \"off\"",
            "invalid initial state") == false)
    {
        return 1;
    }

    if (expect_parse_failure(
            R"({
                "devices": [
                    {
                        "name": "relay-1",
                        "type": "simulated_relay",
                        "initial_state": true
                    }
                ]
            })",
            "device initial_state must be a string",
            "wrong initial-state type") == false)
    {
        return 1;
    }

    std::cout
        << "PASS: device configuration JSON tests\n";

    return 0;
}
