#pragma once

#include "relay.h"

class SimulatedRelay : public IRelay
{
public:
    void turn_on() override;
    void turn_off() override;
    bool is_on() const override;

private:
    bool is_on_{false};
};


