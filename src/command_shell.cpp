

#include "command_shell.h"

void print_help(std::ostream& output)
{
    output << "Available commands:\n";
    output << "  help    Show available commands\n";
    output << "  status  Show platform status\n";
    output << "  exit    Exit the application\n";
}

bool handle_command(
    std::string command,
    IRelay& relay,
    std::ostream& output)
{
    if (command == "exit")
    {
        output << "Shutting down the hardware test platform project.\n";
        return false;
    }
    else if (command == "help")
    {
        print_help(output);
        return true;
    }
    else if (command == "relay on")
    {
        relay.turn_on();
        output << "Relay state: on\n";
        return true;
    }
    else if (command == "relay off")
    {
        relay.turn_off();
        output << "Relay state: off\n";
        return true;
    }
    else if (command == "status")
    {
        output << "Platform status: ready\n";
        return true;
    }
    else
    {
        output << "Unknown command: " << command << '\n';
        return true;
    }
}


