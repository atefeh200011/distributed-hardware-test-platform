#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "command_shell.h"
#include "relay_registry.h"
#include "simulated_relay.h"

namespace
{
bool expect_equal(
    const std::string& actual,
    const std::string& expected,
    const std::string& test_name)
{
    if (actual != expected)
    {
        std::cerr << "FAIL: " << test_name << '\n';
        std::cerr << "Expected:\n" << expected;
        std::cerr << "Actual:\n" << actual;
        return false;
    }

    return true;
}
}

int main()
{
    RelayRegistry relays;

    const auto relay_1 =
        std::make_shared<SimulatedRelay>();

    const auto relay_2 =
        std::make_shared<SimulatedRelay>();

    if (relays.add("relay-1", relay_1) == false ||
        relays.add("relay-2", relay_2) == false)
    {
        std::cerr << "FAIL: test relays should be registered\n";
        return 1;
    }

    std::ostringstream status_output;

    if (handle_command(
            "status",
            relays,
            status_output) == false)
    {
        std::cerr << "FAIL: status should continue\n";
        return 1;
    }

    if (expect_equal(
            status_output.str(),
            "Platform status: ready\n",
            "platform status output") == false)
    {
        return 1;
    }

    std::ostringstream list_output;

    handle_command(
        "relays",
        relays,
        list_output);

    if (expect_equal(
            list_output.str(),
            "Available relays:\n"
            "  relay-1\n"
            "  relay-2\n",
            "relay list output") == false)
    {
        return 1;
    }

    std::ostringstream relay_on_output;

    handle_command(
        "relay relay-1 on",
        relays,
        relay_on_output);

    if (relay_1->is_on() == false)
    {
        std::cerr << "FAIL: relay-1 should be on\n";
        return 1;
    }

    if (relay_2->is_on())
    {
        std::cerr << "FAIL: relay-2 should remain off\n";
        return 1;
    }

    if (expect_equal(
            relay_on_output.str(),
            "Relay relay-1 state: on\n",
            "named relay on output") == false)
    {
        return 1;
    }

    std::ostringstream relay_status_output;

    handle_command(
        "relay relay-1 status",
        relays,
        relay_status_output);

    if (expect_equal(
            relay_status_output.str(),
            "Relay relay-1 state: on\n",
            "named relay status output") == false)
    {
        return 1;
    }

    std::ostringstream relay_off_output;

    handle_command(
        "relay relay-1 off",
        relays,
        relay_off_output);

    if (relay_1->is_on())
    {
        std::cerr << "FAIL: relay-1 should be off\n";
        return 1;
    }

    if (expect_equal(
            relay_off_output.str(),
            "Relay relay-1 state: off\n",
            "named relay off output") == false)
    {
        return 1;
    }

    std::ostringstream second_relay_output;

    handle_command(
        "relay relay-2 on",
        relays,
        second_relay_output);

    if (relay_2->is_on() == false)
    {
        std::cerr << "FAIL: relay-2 should be on\n";
        return 1;
    }

    if (relay_1->is_on())
    {
        std::cerr << "FAIL: relay-1 should remain off\n";
        return 1;
    }

    std::ostringstream missing_output;

    handle_command(
        "relay missing status",
        relays,
        missing_output);

    if (expect_equal(
            missing_output.str(),
            "Relay not found: missing\n",
            "missing relay output") == false)
    {
        return 1;
    }

    std::ostringstream invalid_output;

    handle_command(
        "relay relay-1 invalid",
        relays,
        invalid_output);

    if (expect_equal(
            invalid_output.str(),
            "Unknown command: relay relay-1 invalid\n",
            "invalid relay action output") == false)
    {
        return 1;
    }

    std::ostringstream exit_output;

    if (handle_command(
            "exit",
            relays,
            exit_output))
    {
        std::cerr << "FAIL: exit should stop the shell\n";
        return 1;
    }

    if (expect_equal(
            exit_output.str(),
            "Shutting down the hardware test platform project.\n",
            "exit output") == false)
    {
        return 1;
    }

    std::cout << "PASS: command tests\n";
    return 0;
}
