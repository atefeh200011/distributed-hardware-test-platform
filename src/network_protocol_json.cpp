#include "network_protocol_json.h"

#include <exception>
#include <string>

#include <nlohmann/json.hpp>

std::string serialize_command_request(
    const CommandRequest& request)
{
    const nlohmann::json json_request{
        {"request_id", request.request_id},
        {"command", request.command}
    };

    return json_request.dump();
}

bool parse_command_request(
    const std::string& json_text,
    CommandRequest& request,
    std::string& error_message)
{
    try
    {
        const nlohmann::json json_request =
            nlohmann::json::parse(json_text);

        if (json_request.is_object() == false)
        {
            error_message = "request must be a JSON object";
            return false;
        }

        if (json_request.contains("request_id") == false ||
            json_request["request_id"].is_string() == false)
        {
            error_message = "request_id must be a string";
            return false;
        }

        if (json_request.contains("command") == false ||
            json_request["command"].is_string() == false)
        {
            error_message = "command must be a string";
            return false;
        }

        const std::string request_id =
            json_request["request_id"].get<std::string>();

        const std::string command =
            json_request["command"].get<std::string>();

        if (request_id.empty())
        {
            error_message = "request_id must not be empty";
            return false;
        }

        if (command.empty())
        {
            error_message = "command must not be empty";
            return false;
        }

        request = CommandRequest{
            request_id,
            command
        };

        error_message.clear();
        return true;
    }
    catch (const std::exception& exception)
    {
        error_message = exception.what();
        return false;
    }
}
