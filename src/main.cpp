#include <iostream>
#include <string>

#include "command_shell.h"
#include "simulated_relay.h"

int main()
{
    std::cout << "Hardware Test Platform version 0.1.0\n";

    std::string command;
    SimulatedRelay relay;
    
    while (true)
    {
        std::cout << "hwtest> ";
        std::getline(std::cin, command);

        bool should_continue = handle_command(command, relay, std::cout);

        if (should_continue == false)
        {
            break;
        }
    }

    return 0;
}
