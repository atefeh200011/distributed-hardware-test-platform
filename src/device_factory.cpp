#include "device_factory.h"

#include <memory>
#include <string>
#include <utility>

#include "simulated_relay.h"

bool build_relay_registry(
    const PlatformConfiguration& configuration,
    RelayRegistry& registry,
    std::string& error_message)
{
    if (configuration.devices.empty())
    {
        error_message =
            "configuration must contain at least one device";
        return false;
    }

    RelayRegistry configured_registry;

    for (const DeviceConfiguration& device :
         configuration.devices)
    {
        if (device.name.empty())
        {
            error_message =
                "device name must not be empty";
            return false;
        }

        if (device.type ==
            DeviceType::simulated_relay)
        {
            auto relay =
                std::make_shared<SimulatedRelay>();

            if (device.initial_state)
            {
                relay->turn_on();
            }

            if (configured_registry.add(
                    device.name,
                    relay) == false)
            {
                error_message =
                    "could not register device: " +
                    device.name;
                return false;
            }

            continue;
        }

        error_message =
            "unsupported device type for: " +
            device.name;
        return false;
    }

    registry = std::move(configured_registry);
    error_message.clear();
    return true;
}
