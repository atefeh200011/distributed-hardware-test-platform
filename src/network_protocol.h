#pragma once

#include <string>

struct CommandRequest
{
    std::string request_id;
    std::string command;
};

struct CommandResponse
{
    std::string request_id;
    bool success;
    std::string output;
};
