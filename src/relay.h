#pragma once

class IRelay
{
public:
    virtual ~IRelay() = default;

    virtual void turn_on() = 0;
    virtual void turn_off() = 0;
    virtual bool is_on() const = 0;
};

