#include <iostream>

#include "command_shell.h"

void print_help(std::ostream& output)
{
    output << "Available commands:\n";
    output << "  help    Show available commands\n";
    output << "  status  Show platform status\n";
    output << "  exit    Exit the application\n";
}

bool handle_command(std::string command)
{
    if (command == "exit")
    {
        std::cout << "Shutting down the hardware test platform project.\n";
        return false;
    }
    else if (command == "help")
    {
        print_help(std::cout);
        return true;
    }
    else if (command == "status")
    {
        std::cout << "Platform status: ready\n";
        return true;
    }
    else
    {
        std::cout << "Unknown command: " << command << '\n';
        return true;
    }
}
