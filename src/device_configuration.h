#pragma once

#include <string>
#include <vector>

enum class DeviceType
{
    simulated_relay
};

struct DeviceConfiguration
{
    std::string name;
    DeviceType type;
    bool initial_state{false};
};

struct PlatformConfiguration
{
    std::vector<DeviceConfiguration> devices;
};
