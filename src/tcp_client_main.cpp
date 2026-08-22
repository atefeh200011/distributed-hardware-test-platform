#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tcp_transport.h"
#include "network_protocol.h"
#include "network_protocol_json.h"

namespace
{
constexpr int server_port = 5050;



std::string combine_command_arguments(
    int argument_count,
    char* argument_values[])
{
    std::string command;

    for (int index = 1;
         index < argument_count;
         ++index)
    {
        if (command.empty() == false)
        {
            command += ' ';
        }

        command += argument_values[index];
    }

    return command;
}
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr
            << "Usage: hwtest_client <command>\n"
            << "Example: hwtest_client relay status\n";
        return 1;
    }

    const std::string command =
        combine_command_arguments(argc, argv);

    const CommandRequest request{
        "client-request-001",
        command
    };

    const std::string request_message =
        serialize_command_request(request);

    const int client_socket =
        socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0)
    {
        std::cerr
            << "Failed to create client socket: "
            << std::strerror(errno)
            << '\n';
        return 1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &server_address.sin_addr) != 1)
    {
        std::cerr << "Failed to configure server address\n";
        close(client_socket);
        return 1;
    }

    if (connect(
            client_socket,
            reinterpret_cast<const sockaddr*>(
                &server_address),
            sizeof(server_address)) < 0)
    {
        std::cerr
            << "Failed to connect to 127.0.0.1:"
            << server_port
            << ": "
            << std::strerror(errno)
            << '\n';
        close(client_socket);
        return 1;
    }

    std::string transport_error;

    if (send_framed_message(
            client_socket,
            request_message,
            transport_error) == false)
    {
        std::cerr
            << "Failed to send client request: "
            << transport_error
            << '\n';
        close(client_socket);
        return 1;
    }

    std::string response_message;

    if (receive_framed_message(
            client_socket,
            response_message,
            transport_error) == false)
    {
        std::cerr
            << "Failed to receive server response: "
            << transport_error
            << '\n';
        close(client_socket);
        return 1;
    }

    close(client_socket);

    CommandResponse response;
    std::string parse_error;

    if (parse_command_response(
            response_message,
            response,
            parse_error) == false)
    {
        std::cerr
            << "Failed to parse server response: "
            << parse_error
            << '\n';
        return 1;
    }

    if (response.request_id != request.request_id)
    {
        std::cerr
            << "Response request ID does not match request\n";
        return 1;
    }

    std::cout << response.output;

    if (response.success == false)
    {
        return 2;
    }

    return 0;
}
