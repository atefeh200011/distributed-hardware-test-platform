#include <iostream>
#include <sstream>
#include <string>

#include "command_shell.h"
#include "simulated_relay.h"

int main()
{
    std::ostringstream output;
    SimulatedRelay relay;

    bool should_continue = handle_command("status", relay, output);

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

    std::ostringstream relay_on_output;

    bool relay_on_should_continue =
        handle_command("relay on", relay, relay_on_output);

    if (relay_on_should_continue == false)
    {
        std::cerr << "FAIL: relay on should keep the shell running\n";
        return 1;
    }

    if (relay.is_on() == false)
    {
        std::cerr << "FAIL: relay on should switch the relay on\n";
        return 1;
    }

    const std::string expected_relay_on_output = "Relay state: on\n";

    if (relay_on_output.str() != expected_relay_on_output)
    {
        std::cerr << "FAIL: relay on produced unexpected output\n";
        return 1;
    }

    std::ostringstream exit_output;

    bool exit_should_continue = handle_command("exit", relay, exit_output);

    if (exit_should_continue == true)
    {
        std::cerr << "FAIL: exit should stop the shell\n";
        return 1;
    }

    const std::string expected_exit_output =
        "Shutting down the hardware test platform project.\n";

    if (exit_output.str() != expected_exit_output)
    {
        std::cerr << "FAIL: exit produced unexpected output\n";
        return 1;
    }

    std::cout << "PASS: command tests\n";
    return 0;
}
