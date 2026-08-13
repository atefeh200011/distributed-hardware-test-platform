#include "test_procedure_json.h"

#include <exception>
#include <string>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

namespace
{

bool parse_action(
    const std::string& action_text,
    TestAction& action)
{
    if (action_text == "relay_on")
    {
        action = TestAction::relay_on;
        return true;
    }

    if (action_text == "relay_off")
    {
        action = TestAction::relay_off;
        return true;
    }

    if (action_text == "expect_relay_on")
    {
        action = TestAction::expect_relay_on;
        return true;
    }

    if (action_text == "expect_relay_off")
    {
        action = TestAction::expect_relay_off;
        return true;
    }

    return false;
}

} // namespace

bool parse_test_procedure_json(
    const std::string& json_text,
    TestProcedure& procedure,
    std::string& error_message)
{
    try
    {
        const nlohmann::json document =
            nlohmann::json::parse(json_text);

        if (document.contains("name") == false ||
            document["name"].is_string() == false)
        {
            error_message = "procedure name must be a string";
            return false;
        }

        if (document.contains("steps") == false ||
            document["steps"].is_array() == false)
        {
            error_message = "procedure steps must be an array";
            return false;
        }

        TestProcedure parsed_procedure;
        parsed_procedure.name = document["name"].get<std::string>();

        for (const nlohmann::json& json_step : document["steps"])
        {
            if (json_step.contains("name") == false ||
                json_step["name"].is_string() == false)
            {
                error_message = "step name must be a string";
                return false;
            }

            if (json_step.contains("action") == false ||
                json_step["action"].is_string() == false)
            {
                error_message = "step action must be a string";
                return false;
            }

            TestAction action;
            const std::string action_text =
                json_step["action"].get<std::string>();

            if (parse_action(action_text, action) == false)
            {
                error_message = "unknown test action: " + action_text;
                return false;
            }

            parsed_procedure.steps.push_back(
                {json_step["name"].get<std::string>(), action});
        }

        procedure = parsed_procedure;
        error_message.clear();
        return true;
    }
    catch (const std::exception& error)
    {
        error_message = error.what();
        return false;
    }
}

bool load_test_procedure_file(
    const std::string& file_path,
    TestProcedure& procedure,
    std::string& error_message)
{
    std::ifstream file{file_path};

    if (file.is_open() == false)
    {
        error_message = "could not open file: " + file_path;
        return false;
    }

    std::ostringstream contents;
    contents << file.rdbuf();

    return parse_test_procedure_json(
        contents.str(),
        procedure,
        error_message);
}