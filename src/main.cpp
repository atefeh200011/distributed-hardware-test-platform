#include <iostream>
#include <string>

int main()
{
    std::cout << "Hardware Test Platform version 0.1.0\n";
    std::cout << "hwtest> ";

    std::string command;
    std::getline(std::cin, command);

    if (command == "status")
    {
        std::cout << "Platform status: ready\n";
    }
    else
    {
        std::cout << "Unknown command: " << command << '\n';
    }
    return 0;
}
