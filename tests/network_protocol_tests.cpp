#include <iostream>
#include <string>

#include "network_protocol.h"
#include "network_protocol_json.h"

int main()
{
    const CommandRequest original_request{
        "request-001",
        "relay on"
    };

    const std::string json_text =
        serialize_command_request(original_request);

    CommandRequest parsed_request;
    std::string error_message;

    const bool parsed = parse_command_request(
        json_text,
        parsed_request,
        error_message);

    if (parsed == false)
    {
        std::cerr << "FAIL: serialized request should parse\n";
        std::cerr << "Error: " << error_message << '\n';
        return 1;
    }

    if (parsed_request.request_id !=
        original_request.request_id)
    {
        std::cerr << "FAIL: parsed request ID is incorrect\n";
        return 1;
    }

    if (parsed_request.command != original_request.command)
    {
        std::cerr << "FAIL: parsed command is incorrect\n";
        return 1;
    }

    const std::string missing_command_json = R"(
{
    "request_id": "request-002"
}
)";

    CommandRequest invalid_request;
    std::string invalid_error;

    const bool invalid_parsed = parse_command_request(
        missing_command_json,
        invalid_request,
        invalid_error);

    if (invalid_parsed)
    {
        std::cerr
            << "FAIL: request without command should be rejected\n";
        return 1;
    }

    if (invalid_error != "command must be a string")
    {
        std::cerr << "FAIL: invalid request error is incorrect\n";
        std::cerr << "Actual error: " << invalid_error << '\n';
        return 1;
    }

    const std::string empty_command_json = R"(
{
    "request_id": "request-003",
    "command": ""
}
)";

    CommandRequest empty_command_request;
    std::string empty_command_error;

    const bool empty_command_parsed = parse_command_request(
        empty_command_json,
        empty_command_request,
        empty_command_error);

    if (empty_command_parsed)
    {
        std::cerr << "FAIL: empty command should be rejected\n";
        return 1;
    }

    if (empty_command_error != "command must not be empty")
    {
        std::cerr << "FAIL: empty command error is incorrect\n";
        return 1;
    }

    std::cout << "PASS: network request JSON protocol\n";
    return 0;
}
