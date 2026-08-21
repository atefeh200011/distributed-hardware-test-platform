#pragma once

#include <string>

std::string frame_message(const std::string& message);

bool extract_next_message(
    std::string& receive_buffer,
    std::string& message);
    #pragma once

#include <string>

std::string frame_message(const std::string& message);

bool extract_next_message(
    std::string& receive_buffer,
    std::string& message);
    