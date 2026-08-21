#pragma once

#include <string>

#include "network_protocol.h"

std::string serialize_command_request(
    const CommandRequest& request);

bool parse_command_request(
    const std::string& json_text,
    CommandRequest& request,
    std::string& error_message);

std::string serialize_command_response(
    const CommandResponse& response);

bool parse_command_response(
    const std::string& json_text,
    CommandResponse& response,
    std::string& error_message);
    