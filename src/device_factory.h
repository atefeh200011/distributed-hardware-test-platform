#pragma once

#include <string>

#include "device_configuration.h"
#include "relay_registry.h"

bool build_relay_registry(
    const PlatformConfiguration& configuration,
    RelayRegistry& registry,
    std::string& error_message);
    