#pragma once

#include <ostream>
#include <string>

#include "relay.h"
#include "relay_registry.h"

void print_help(std::ostream& output);

bool handle_command(
    const std::string& command,
    RelayRegistry& relays,
    std::ostream& output);

bool handle_command(
    const std::string& command,
    IRelay& relay,
    std::ostream& output);
