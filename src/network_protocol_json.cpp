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

std::string serialize_command_response(
    const CommandResponse& response)
{
    const nlohmann::json json_response{
        {"request_id", response.request_id},
        {"success", response.success},
        {"output", response.output}
    };

    return json_response.dump();
}

bool parse_command_response(
    const std::string& json_text,
    CommandResponse& response,
    std::string& error_message)
{
    try
    {
        const nlohmann::json json_response =
            nlohmann::json::parse(json_text);

        if (json_response.is_object() == false)
        {
            error_message = "response must be a JSON object";
            return false;
        }

        if (json_response.contains("request_id") == false ||
            json_response["request_id"].is_string() == false)
        {
            error_message = "request_id must be a string";
            return false;
        }

        if (json_response.contains("success") == false ||
            json_response["success"].is_boolean() == false)
        {
            error_message = "success must be a boolean";
            return false;
        }

        if (json_response.contains("output") == false ||
            json_response["output"].is_string() == false)
        {
            error_message = "output must be a string";
            return false;
        }

        const std::string request_id =
            json_response["request_id"].get<std::string>();

        if (request_id.empty())
        {
            error_message = "request_id must not be empty";
            return false;
        }

        const bool success =
            json_response["success"].get<bool>();

        const std::string output =
            json_response["output"].get<std::string>();

        response = CommandResponse{
            request_id,
            success,
            output
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
