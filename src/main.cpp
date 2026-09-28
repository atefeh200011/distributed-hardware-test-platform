#include <iostream>
#include <memory>
#include <string>

#include "command_shell.h"
#include "relay_registry.h"
#include "simulated_relay.h"

int main()
{
    std::cout
        << "Hardware Test Platform version 0.1.0\n";

    RelayRegistry relays;

    relays.add(
        "relay-1",
        std::make_shared<SimulatedRelay>());

    relays.add(
        "relay-2",
        std::make_shared<SimulatedRelay>());

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
