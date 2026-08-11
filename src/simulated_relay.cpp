#include "simulated_relay.h"

void SimulatedRelay::turn_on()
{
    is_on_ = true;
}

void SimulatedRelay::turn_off()
{
    is_on_ = false;
}

bool SimulatedRelay::is_on() const
{
    return is_on_;
}

