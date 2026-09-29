#include "device_configuration_json.h"

#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

#include <nlohmann/json.hpp>

namespace
{
bool parse_device_type(
    const std::string& type_text,
    DeviceType& type)
{
    if (type_text == "simulated_relay")
    {
        type = DeviceType::simulated_relay;
        return true;
    }

    return false;
}

bool parse_initial_state(
    const std::string& state_text,
    bool& initial_state)
{
    if (state_text == "off")
    {
        initial_state = false;
        return true;
    }

    if (state_text == "on")
    {
        initial_state = true;
        return true;
    }

    return false;
}
}

bool parse_device_configuration_json(
    const std::string& json_text,
    PlatformConfiguration& configuration,
    std::string& error_message)
{
    try
    {
        const nlohmann::json parsed_json =
            nlohmann::json::parse(json_text);

        if (parsed_json.is_object() == false)
        {
            error_message =
                "configuration must be a JSON object";
            return false;
        }

        if (parsed_json.contains("devices") == false ||
            parsed_json["devices"].is_array() == false)
        {
            error_message =
                "configuration devices must be an array";
            return false;
        }

        if (parsed_json["devices"].empty())
        {
            error_message =
                "configuration must contain at least one device";
            return false;
        }

        PlatformConfiguration parsed_configuration;
        std::unordered_set<std::string> device_names;

        for (const auto& device_json :
             parsed_json["devices"])
        {
            if (device_json.is_object() == false)
            {
                error_message =
                    "device must be a JSON object";
                return false;
            }

            if (device_json.contains("name") == false ||
                device_json["name"].is_string() == false)
            {
                error_message =
                    "device name must be a string";
                return false;
            }

            const std::string name =
                device_json["name"].get<std::string>();

            if (name.empty())
            {
                error_message =
                    "device name must not be empty";
                return false;
            }

            if (device_names.contains(name))
            {
                error_message =
                    "duplicate device name: " + name;
                return false;
            }

            if (device_json.contains("type") == false ||
                device_json["type"].is_string() == false)
            {
                error_message =
                    "device type must be a string";
                return false;
            }

            const std::string type_text =
                device_json["type"].get<std::string>();

            DeviceType type;

            if (parse_device_type(
                    type_text,
                    type) == false)
            {
                error_message =
                    "unknown device type: " +
                    type_text;
                return false;
            }

            bool initial_state = false;

            if (device_json.contains("initial_state"))
            {
                if (device_json["initial_state"]
                        .is_string() == false)
                {
                    error_message =
                        "device initial_state must be a string";
                    return false;
                }

                const std::string state_text =
                    device_json["initial_state"]
                        .get<std::string>();

                if (parse_initial_state(
                        state_text,
                        initial_state) == false)
                {
                    error_message =
                        "device initial_state must be "
                        "\"on\" or \"off\"";
                    return false;
                }
            }

            device_names.insert(name);

            parsed_configuration.devices.push_back(
                DeviceConfiguration{
                    name,
                    type,
                    initial_state
                });
        }

        configuration =
            std::move(parsed_configuration);

        error_message.clear();
        return true;
    }
    catch (const std::exception& exception)
    {
        error_message = exception.what();
        return false;
    }
}

bool load_device_configuration_file(
    const std::string& file_path,
    PlatformConfiguration& configuration,
    std::string& error_message)
{
    std::ifstream input_file(file_path);

    if (input_file.is_open() == false)
    {
        error_message =
            "could not open device configuration file: " +
            file_path;
        return false;
    }

    std::ostringstream json_text;
    json_text << input_file.rdbuf();

    return parse_device_configuration_json(
        json_text.str(),
        configuration,
        error_message);
}
