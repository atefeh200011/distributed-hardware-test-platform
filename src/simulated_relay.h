#pragma once

class SimulatedRelay
{
public:
    void turn_on();
    void turn_off();
    bool is_on() const;

private:
    bool is_on_{false};
};

