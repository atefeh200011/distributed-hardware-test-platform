#include <iostream>
#include <string>

#include "command_shell.h"
#include "device_configuration.h"
#include "device_configuration_json.h"
#include "device_factory.h"
#include "relay_registry.h"
#include "version.h"

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr
            << "Usage: hwtest [device-configuration-file]\n";
        return 1;
    }

    const std::string configuration_file =
        argc == 2
            ? argv[1]
            : "config/devices.json";

    PlatformConfiguration configuration;
    std::string error_message;

    if (load_device_configuration_file(
            configuration_file,
            configuration,
            error_message) == false)
    {
        std::cerr
            << "Failed to load device configuration: "
            << error_message
            << '\n';
        return 1;
    }

    RelayRegistry relays;

    if (build_relay_registry(
            configuration,
            relays,
            error_message) == false)
    {
        std::cerr
            << "Failed to build device registry: "
            << error_message
            << '\n';
        return 1;
    }

    std::cout
        << "Hardware Test Platform version "
        << hwtest_version
        << '\n';
    std::cout
        << "Loaded "
        << relays.size()
        << " configured devices from "
        << configuration_file
        << '\n';

    std::string command;

    while (true)
    {
        std::cout << "hwtest> ";

        if (!std::getline(std::cin, command))
        {
            std::cout << '\n';
            break;
        }

        const bool should_continue =
            handle_command(
                command,
                relays,
                std::cout);

        if (should_continue == false)
        {
            break;
        }
    }

    return 0;
}
