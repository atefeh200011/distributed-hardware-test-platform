#include <iostream>
#include <string>

int main()
{
    std::cout << "Hardware Test Platform version 0.1.0\n";

    std::string command;

    while (true)
    {
        std::cout << "hwtest> ";
        std::getline(std::cin, command);

        if (command == "exit")
        {
            std::cout << "Shutting down the hardware test platform project.\n";
            break;
        }
        else if (command == "help")
        {
            std::cout << "Available commands:\n";
            std::cout << "  help    Show available commands\n";
            std::cout << "  status  Show platform status\n";
            std::cout << "  exit    Exit the application\n";
        }
        else if (command == "status")
        {
            std::cout << "Platform status: ready\n";
        }
        else
        {
            std::cout << "Unknown command: " << command << '\n';
        }
    }
 
    return 0;
}
