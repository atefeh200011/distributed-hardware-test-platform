#include <iostream>

#include "command_shell.h"

void print_help()
{
    std::cout << "Available commands:\n";
    std::cout << "  help    Show available commands\n";
    std::cout << "  status  Show platform status\n";
    std::cout << "  exit    Exit the application\n";
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
        print_help();
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
