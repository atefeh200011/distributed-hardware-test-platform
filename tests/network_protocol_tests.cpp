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

    const CommandResponse original_response{
        "request-004",
        true,
        "Relay state: on\n"
    };

    const std::string response_json =
        serialize_command_response(original_response);

    CommandResponse parsed_response;
    std::string response_error;

    const bool response_parsed = parse_command_response(
        response_json,
        parsed_response,
        response_error);

    if (response_parsed == false)
    {
        std::cerr << "FAIL: serialized response should parse\n";
        std::cerr << "Error: " << response_error << '\n';
        return 1;
    }

    if (parsed_response.request_id !=
        original_response.request_id)
    {
        std::cerr << "FAIL: parsed response ID is incorrect\n";
        return 1;
    }

    if (parsed_response.success != original_response.success)
    {
        std::cerr
            << "FAIL: parsed response success value is incorrect\n";
        return 1;
    }

    if (parsed_response.output != original_response.output)
    {
        std::cerr << "FAIL: parsed response output is incorrect\n";
        return 1;
    }

    const std::string invalid_response_json = R"(
    {
        "request_id": "request-005",
        "success": "yes",
        "output": "Invalid response"
    }
    )";

    CommandResponse invalid_response;
    std::string invalid_response_error;

    const bool invalid_response_parsed =
        parse_command_response(
            invalid_response_json,
            invalid_response,
            invalid_response_error);

    if (invalid_response_parsed)
    {
        std::cerr
            << "FAIL: non-Boolean success should be rejected\n";
        return 1;
    }

    if (invalid_response_error != "success must be a boolean")
    {
        std::cerr
            << "FAIL: invalid response error is incorrect\n";
        std::cerr << "Actual error: "
                << invalid_response_error << '\n';
        return 1;
    }

    std::cout << "PASS: network JSON protocol\n";
    return 0;
}
