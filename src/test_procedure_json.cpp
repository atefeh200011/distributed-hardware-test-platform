#include "test_procedure_json.h"

#include <cstddef>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

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
}

bool parse_test_procedure_json(
    const std::string& json_text,
    TestProcedure& procedure,
    std::string& error_message)
{
    try
    {
        const nlohmann::json parsed_json =
            nlohmann::json::parse(json_text);

        if (parsed_json.contains("name") == false ||
            parsed_json["name"].is_string() == false)
        {
            error_message = "procedure name must be a string";
            return false;
        }

        if (parsed_json.contains("steps") == false ||
            parsed_json["steps"].is_array() == false)
        {
            error_message = "procedure steps must be an array";
            return false;
        }

        TestProcedure parsed_procedure;
        parsed_procedure.name =
            parsed_json["name"].get<std::string>();

        for (const auto& step_json : parsed_json["steps"])
        {
            if (step_json.contains("name") == false ||
                step_json["name"].is_string() == false)
            {
                error_message = "step name must be a string";
                return false;
            }

            if (step_json.contains("action") == false ||
                step_json["action"].is_string() == false)
            {
                error_message = "step action must be a string";
                return false;
            }

            const std::string step_name =
                step_json["name"].get<std::string>();

            const std::string action_text =
                step_json["action"].get<std::string>();

            TestAction action;

            if (parse_action(action_text, action) == false)
            {
                error_message =
                    "unknown test action: " + action_text;
                return false;
            }

            std::size_t retries = 0;

            if (step_json.contains("retries"))
            {
                if (step_json["retries"].is_number_unsigned() == false)
                {
                    error_message =
                        "step retries must be a non-negative integer";
                    return false;
                }

                retries =
                    step_json["retries"].get<std::size_t>();
            }

            parsed_procedure.steps.push_back(
                TestStep{step_name, action, retries});
        }

        procedure = parsed_procedure;
        error_message.clear();
        return true;
    }
    catch (const std::exception& exception)
    {
        error_message = exception.what();
        return false;
    }
}

bool load_test_procedure_file(
    const std::string& file_path,
    TestProcedure& procedure,
    std::string& error_message)
{
    std::ifstream input_file(file_path);

    if (input_file.is_open() == false)
    {
        error_message =
            "could not open procedure file: " + file_path;
        return false;
    }

    std::ostringstream json_text;
    json_text << input_file.rdbuf();

    return parse_test_procedure_json(
        json_text.str(),
        procedure,
        error_message);
}