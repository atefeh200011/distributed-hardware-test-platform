#include <iostream>

#include "simulated_relay.h"

int main()
{
    SimulatedRelay simulated_relay;
    IRelay& relay = simulated_relay;

    if (relay.is_on() == true)
    {
        std::cerr << "FAIL: a new relay should be off\n";
        return 1;
    }

    relay.turn_on();

    if (relay.is_on() == false)
    {
        std::cerr << "FAIL: turn_on should switch the relay on\n";
        return 1;
    }

    relay.turn_on();

    if (relay.is_on() == false)
    {
        std::cerr << "FAIL: repeated turn_on should keep the relay on\n";
        return 1;
    }

    relay.turn_off();

    if (relay.is_on() == true)
    {
        std::cerr << "FAIL: turn_off should switch the relay off\n";
        return 1;
    }

    std::cout << "PASS: simulated relay tests\n";
    return 0;
}
