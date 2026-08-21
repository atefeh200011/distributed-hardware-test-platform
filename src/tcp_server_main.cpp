#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network_framing.h"
#include "network_protocol.h"
#include "network_protocol_json.h"

namespace
{
constexpr int server_port = 5050;
constexpr std::size_t maximum_message_size = 64 * 1024;

bool send_all(int socket_descriptor, const std::string& data)
{
    std::size_t total_sent = 0;

    while (total_sent < data.size())
    {
        const ssize_t sent = send(
            socket_descriptor,
            data.data() + total_sent,
            data.size() - total_sent,
            MSG_NOSIGNAL);

        if (sent < 0)
        {
            std::cerr
                << "Server send failed: "
                << std::strerror(errno)
                << '\n';
            return false;
        }

        total_sent += static_cast<std::size_t>(sent);
    }

    return true;
}
}

int main()
{
    const int server_socket =
        socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        std::cerr
            << "Failed to create server socket: "
            << std::strerror(errno)
            << '\n';
        return 1;
    }

    const int reuse_address = 1;

    if (setsockopt(
            server_socket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse_address,
            sizeof(reuse_address)) < 0)
    {
        std::cerr
            << "Failed to configure server socket: "
            << std::strerror(errno)
            << '\n';
        close(server_socket);
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
        close(server_socket);
        return 1;
    }

    if (bind(
            server_socket,
            reinterpret_cast<const sockaddr*>(
                &server_address),
            sizeof(server_address)) < 0)
    {
        std::cerr
            << "Failed to bind server to 127.0.0.1:"
            << server_port
            << ": "
            << std::strerror(errno)
            << '\n';
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 1) < 0)
    {
        std::cerr
            << "Failed to listen for clients: "
            << std::strerror(errno)
            << '\n';
        close(server_socket);
        return 1;
    }

    std::cout
        << "Hardware test server listening on 127.0.0.1:"
        << server_port
        << '\n';

    const int client_socket =
        accept(server_socket, nullptr, nullptr);

    if (client_socket < 0)
    {
        std::cerr
            << "Failed to accept client: "
            << std::strerror(errno)
            << '\n';
        close(server_socket);
        return 1;
    }

    std::string receive_buffer;
    std::string request_message;
    char received_data[4096];

    while (extract_next_message(
               receive_buffer,
               request_message) == false)
    {
        const ssize_t received = recv(
            client_socket,
            received_data,
            sizeof(received_data),
            0);

        if (received == 0)
        {
            std::cerr
                << "Client disconnected before sending a message\n";
            close(client_socket);
            close(server_socket);
            return 1;
        }

        if (received < 0)
        {
            std::cerr
                << "Server receive failed: "
                << std::strerror(errno)
                << '\n';
            close(client_socket);
            close(server_socket);
            return 1;
        }

        receive_buffer.append(
            received_data,
            static_cast<std::size_t>(received));

        if (receive_buffer.size() > maximum_message_size)
        {
            std::cerr << "Received message is too large\n";
            close(client_socket);
            close(server_socket);
            return 1;
        }
    }

    CommandRequest request;
    std::string parse_error;
    CommandResponse response;

    if (parse_command_request(
            request_message,
            request,
            parse_error))
    {
        response = CommandResponse{
            request.request_id,
            true,
            "Request received: " + request.command + '\n'
        };
    }
    else
    {
        response = CommandResponse{
            "unknown",
            false,
            "Invalid request: " + parse_error + '\n'
        };
    }

    const std::string response_message =
        frame_message(
            serialize_command_response(response));

    const bool response_sent =
        send_all(client_socket, response_message);

    close(client_socket);
    close(server_socket);

    if (response_sent == false)
    {
        return 1;
    }

    std::cout << "Server handled one request and stopped\n";
    return 0;
}