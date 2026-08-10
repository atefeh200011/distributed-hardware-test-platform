#include <iostream>
#include <sstream>
#include <string>

#include "command_shell.h"

int main()
{
    std::ostringstream output;

    bool should_continue = handle_command("status", output);

    if (should_continue == false)
    {
        std::cerr << "FAIL: status should keep the shell running\n";
        return 1;
    }

    const std::string expected_output = "Platform status: ready\n";

    if (output.str() != expected_output)
    {
        std::cerr << "FAIL: status produced unexpected output\n";
        return 1;
    }

    std::cout << "PASS: status command\n";
    return 0;
}
