#include <iostream>
#include <string>

#include "device_factory.h"

int main()
{
    const PlatformConfiguration configuration{
        {
            {
                "relay-1",
                DeviceType::simulated_relay,
                false
            },
            {
                "relay-2",
                DeviceType::simulated_relay,
                true
            }
        }
    };

    RelayRegistry registry;
    std::string error_message;

    if (build_relay_registry(
            configuration,
            registry,
            error_message) == false)
    {
        std::cerr
            << "FAIL: valid configuration was rejected\n";
        std::cerr
            << "Error: "
            << error_message
            << '\n';
        return 1;
    }

    if (registry.size() != 2)
    {
        std::cerr
            << "FAIL: registry should contain two devices\n";
        return 1;
    }

    IRelay* relay_1 = registry.find("relay-1");
    IRelay* relay_2 = registry.find("relay-2");

    if (relay_1 == nullptr ||
        relay_2 == nullptr)
    {
        std::cerr
            << "FAIL: configured relays should be found\n";
        return 1;
    }

    if (relay_1->is_on())
    {
        std::cerr
            << "FAIL: relay-1 should initially be off\n";
        return 1;
    }

    if (relay_2->is_on() == false)
    {
        std::cerr
            << "FAIL: relay-2 should initially be on\n";
        return 1;
    }

    relay_1->turn_on();

    if (relay_1->is_on() == false)
    {
        std::cerr
            << "FAIL: configured relay should be controllable\n";
        return 1;
    }

    if (relay_2->is_on() == false)
    {
        std::cerr
            << "FAIL: relay states should remain independent\n";
        return 1;
    }

    const PlatformConfiguration duplicate_configuration{
        {
            {
                "duplicate",
                DeviceType::simulated_relay,
                false
            },
            {
                "duplicate",
                DeviceType::simulated_relay,
                true
            }
        }
    };

    RelayRegistry duplicate_registry;
    error_message.clear();

    if (build_relay_registry(
            duplicate_configuration,
            duplicate_registry,
            error_message))
    {
        std::cerr
            << "FAIL: duplicate names should be rejected\n";
        return 1;
    }

    if (error_message !=
        "could not register device: duplicate")
    {
        std::cerr
            << "FAIL: duplicate error is incorrect\n";
        std::cerr
            << "Actual: "
            << error_message
            << '\n';
        return 1;
    }

    if (duplicate_registry.size() != 0)
    {
        std::cerr
            << "FAIL: failed build should not partially "
            << "populate the registry\n";
        return 1;
    }

    const PlatformConfiguration unsupported_configuration{
        {
            {
                "unsupported",
                static_cast<DeviceType>(999),
                false
            }
        }
    };

    RelayRegistry unsupported_registry;
    error_message.clear();

    if (build_relay_registry(
            unsupported_configuration,
            unsupported_registry,
            error_message))
    {
        std::cerr
            << "FAIL: unsupported type should be rejected\n";
        return 1;
    }

    if (error_message !=
        "unsupported device type for: unsupported")
    {
        std::cerr
            << "FAIL: unsupported-type error is incorrect\n";
        std::cerr
            << "Actual: "
            << error_message
            << '\n';
        return 1;
    }

    const PlatformConfiguration empty_configuration;

    RelayRegistry empty_registry;
    error_message.clear();

    if (build_relay_registry(
            empty_configuration,
            empty_registry,
            error_message))
    {
        std::cerr
            << "FAIL: empty configuration should be rejected\n";
        return 1;
    }

    if (error_message !=
        "configuration must contain at least one device")
    {
        std::cerr
            << "FAIL: empty-configuration error is incorrect\n";
        return 1;
    }

    std::cout
        << "PASS: device factory tests\n";

    return 0;
}
