#include <iostream>
#include <sstream>
#include <string>

#include "command_shell.h"
#include "simulated_relay.h"

bool expect_equal(
    const std::string& actual,
    const std::string& expected,
    const std::string& test_name)
{
    if (actual != expected)
    {
        std::cerr << "FAIL: " << test_name << '\n';
        std::cerr << "  Expected: " << expected;
        std::cerr << "  Actual:   " << actual;
        return false;
    }

    return true;
}

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

    if (expect_equal(
            output.str(),
            expected_output,
            "status should produce expected output") == false)
    {
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

    if (expect_equal(
            relay_on_output.str(),
            expected_relay_on_output,
            "relay on should produce expected output") == false)
    {
        return 1;
    }

    std::ostringstream relay_status_on_output;

    bool relay_status_on_should_continue =
        handle_command("relay status", relay, relay_status_on_output);

    if (relay_status_on_should_continue == false)
    {
        std::cerr << "FAIL: relay status should keep the shell running\n";
        return 1;
    }

    if (expect_equal(
            relay_status_on_output.str(),
            "Relay state: on\n",
            "relay status should report on") == false)
    {
        return 1;
    }

    std::ostringstream relay_off_output;

    bool relay_off_should_continue =
        handle_command("relay off", relay, relay_off_output);

    if (relay_off_should_continue == false)
    {
        std::cerr << "FAIL: relay off should keep the shell running\n";
        return 1;
    }

    if (relay.is_on() == true)
    {
        std::cerr << "FAIL: relay off should switch the relay off\n";
        return 1;
    }

    const std::string expected_relay_off_output = "Relay state: off\n";

    if (expect_equal(
            relay_off_output.str(),
            expected_relay_off_output,
            "relay off should produce expected output") == false)
    {
        return 1;
    }

    std::ostringstream relay_status_off_output;

    bool relay_status_off_should_continue =
        handle_command("relay status", relay, relay_status_off_output);

    if (relay_status_off_should_continue == false)
    {
        std::cerr << "FAIL: relay status should keep the shell running\n";
        return 1;
    }

    if (expect_equal(
            relay_status_off_output.str(),
            "Relay state: off\n",
            "relay status should report off") == false)
    {
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

    if (expect_equal(
            exit_output.str(),
            expected_exit_output,
            "exit should produce expected output") == false)
    {
        return 1;
    }

    std::cout << "PASS: command tests\n";
    return 0;
}