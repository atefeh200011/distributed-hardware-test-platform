#pragma once

#include <ostream>
#include <string>

#include "relay.h"

void print_help(std::ostream& output);

bool handle_command(
    std::string command,
    IRelay& relay,
    std::ostream& output);
