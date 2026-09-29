#include <iostream>

#include "device_configuration.h"

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

    if (configuration.devices.size() != 2)
    {
        std::cerr
            << "FAIL: configuration should contain "
            << "two devices\n";
        return 1;
    }

    const DeviceConfiguration& first_device =
        configuration.devices[0];

    if (first_device.name != "relay-1")
    {
        std::cerr
            << "FAIL: first device name is incorrect\n";
        return 1;
    }

    if (first_device.type !=
        DeviceType::simulated_relay)
    {
        std::cerr
            << "FAIL: first device type is incorrect\n";
        return 1;
    }

    if (first_device.initial_state)
    {
        std::cerr
            << "FAIL: first device should initially be off\n";
        return 1;
    }

    const DeviceConfiguration& second_device =
        configuration.devices[1];

    if (second_device.name != "relay-2")
    {
        std::cerr
            << "FAIL: second device name is incorrect\n";
        return 1;
    }

    if (second_device.type !=
        DeviceType::simulated_relay)
    {
        std::cerr
            << "FAIL: second device type is incorrect\n";
        return 1;
    }

    if (second_device.initial_state == false)
    {
        std::cerr
            << "FAIL: second device should initially be on\n";
        return 1;
    }

    const DeviceConfiguration default_device{};

    if (default_device.initial_state)
    {
        std::cerr
            << "FAIL: default initial state should be off\n";
        return 1;
    }

    std::cout
        << "PASS: device configuration model\n";

    return 0;
}
