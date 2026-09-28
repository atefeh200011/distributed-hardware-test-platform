#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "relay_registry.h"
#include "simulated_relay.h"

int main()
{
    RelayRegistry registry;

    if (registry.size() != 0)
    {
        std::cerr << "FAIL: a new registry should be empty\n";
        return 1;
    }

    const bool first_added = registry.add(
        "relay-1",
        std::make_shared<SimulatedRelay>());

    if (first_added == false)
    {
        std::cerr << "FAIL: valid relay should be added\n";
        return 1;
    }

    const bool second_added = registry.add(
        "relay-2",
        std::make_shared<SimulatedRelay>());

    if (second_added == false)
    {
        std::cerr << "FAIL: second relay should be added\n";
        return 1;
    }

    if (registry.size() != 2)
    {
        std::cerr << "FAIL: registry should contain two relays\n";
        return 1;
    }

    if (registry.contains("relay-1") == false)
    {
        std::cerr << "FAIL: relay-1 should exist\n";
        return 1;
    }

    IRelay* relay = registry.find("relay-1");

    if (relay == nullptr)
    {
        std::cerr << "FAIL: relay-1 should be found\n";
        return 1;
    }

    relay->turn_on();

    if (relay->is_on() == false)
    {
        std::cerr << "FAIL: found relay should be controllable\n";
        return 1;
    }

    if (registry.find("missing") != nullptr)
    {
        std::cerr << "FAIL: missing relay should return nullptr\n";
        return 1;
    }

    const bool duplicate_added = registry.add(
        "relay-1",
        std::make_shared<SimulatedRelay>());

    if (duplicate_added)
    {
        std::cerr << "FAIL: duplicate relay name should be rejected\n";
        return 1;
    }

    if (registry.add(
            "",
            std::make_shared<SimulatedRelay>()))
    {
        std::cerr << "FAIL: empty relay name should be rejected\n";
        return 1;
    }

    if (registry.add("null-relay", nullptr))
    {
        std::cerr << "FAIL: null relay should be rejected\n";
        return 1;
    }

    const std::vector<std::string> expected_names{
        "relay-1",
        "relay-2"
    };

    if (registry.names() != expected_names)
    {
        std::cerr << "FAIL: relay names should be sorted\n";
        return 1;
    }

    std::cout << "PASS: relay registry tests\n";
    return 0;
}
