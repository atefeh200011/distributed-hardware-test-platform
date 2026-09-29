#pragma once

#include <string>

#include "device_configuration.h"

bool parse_device_configuration_json(
    const std::string& json_text,
    PlatformConfiguration& configuration,
    std::string& error_message);

bool load_device_configuration_file(
    const std::string& file_path,
    PlatformConfiguration& configuration,
    std::string& error_message);
    