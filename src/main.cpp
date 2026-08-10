#include <iostream>
#include <string>

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

int main()
{
    std::cout << "Hardware Test Platform version 0.1.0\n";

    std::string command;

    while (true)
    {
        std::cout << "hwtest> ";
        std::getline(std::cin, command);
        
        bool should_continue = handle_command(command);

        if (should_continue == false)
        {
            break;
        }
    }
 
    return 0;
}
