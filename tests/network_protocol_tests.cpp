#include <iostream>

#include "network_protocol.h"

int main()
{
    const CommandRequest request{
        "request-001",
        "relay on"
    };

    if (request.request_id != "request-001")
    {
        std::cerr << "FAIL: request ID is incorrect\n";
        return 1;
    }

    if (request.command != "relay on")
    {
        std::cerr << "FAIL: request command is incorrect\n";
        return 1;
    }

    const CommandResponse response{
        request.request_id,
        true,
        "Relay state: on\n"
    };

    if (response.request_id != request.request_id)
    {
        std::cerr
            << "FAIL: response ID should match request ID\n";
        return 1;
    }

    if (response.success == false)
    {
        std::cerr << "FAIL: response should indicate success\n";
        return 1;
    }

    if (response.output != "Relay state: on\n")
    {
        std::cerr << "FAIL: response output is incorrect\n";
        return 1;
    }

    std::cout << "PASS: network protocol model\n";
    return 0;
}
