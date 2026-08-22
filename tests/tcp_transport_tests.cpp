#include <iostream>
#include <string>

#include <sys/socket.h>
#include <unistd.h>

#include "tcp_transport.h"

int main()
{
    int sockets[2];

    if (socketpair(
            AF_UNIX,
            SOCK_STREAM,
            0,
            sockets) < 0)
    {
        std::cerr << "FAIL: could not create socket pair\n";
        return 1;
    }

    const std::string original_message =
        R"({"request_id":"request-001","command":"relay on"})";

    std::string send_error;

    if (send_framed_message(
            sockets[0],
            original_message,
            send_error) == false)
    {
        std::cerr << "FAIL: message could not be sent\n";
        std::cerr << "Error: " << send_error << '\n';
        close(sockets[0]);
        close(sockets[1]);
        return 1;
    }

    std::string received_message;
    std::string receive_error;

    if (receive_framed_message(
            sockets[1],
            received_message,
            receive_error) == false)
    {
        std::cerr << "FAIL: message could not be received\n";
        std::cerr << "Error: " << receive_error << '\n';
        close(sockets[0]);
        close(sockets[1]);
        return 1;
    }

    close(sockets[0]);
    close(sockets[1]);

    if (received_message != original_message)
    {
        std::cerr << "FAIL: received message is incorrect\n";
        return 1;
    }

    std::cout << "PASS: TCP transport\n";
    return 0;
}
