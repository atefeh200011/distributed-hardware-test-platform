#pragma once

#include <string>

bool send_framed_message(
    int socket_descriptor,
    const std::string& message,
    std::string& error_message);

bool receive_framed_message(
    int socket_descriptor,
    std::string& message,
    std::string& error_message);
    