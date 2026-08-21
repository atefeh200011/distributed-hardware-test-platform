#include "network_framing.h"

#include <cstddef>
#include <string>

std::string frame_message(const std::string& message)
{
    return message + '\n';
}

bool extract_next_message(
    std::string& receive_buffer,
    std::string& message)
{
    const std::size_t newline_position =
        receive_buffer.find('\n');

    if (newline_position == std::string::npos)
    {
        return false;
    }

    message = receive_buffer.substr(0, newline_position);

    if (message.empty() == false &&
        message.back() == '\r')
    {
        message.pop_back();
    }

    receive_buffer.erase(0, newline_position + 1);

    return true;
}
