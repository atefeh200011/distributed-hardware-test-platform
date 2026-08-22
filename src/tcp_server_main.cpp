#include <arpa/inet.h>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "command_shell.h"
#include "network_protocol.h"
#include "network_protocol_json.h"
#include "simulated_relay.h"
#include "tcp_transport.h"

namespace
{
constexpr int server_port = 5050;

bool handle_client(
    int client_socket,
    int server_socket,
    SimulatedRelay& relay,
    std::mutex& command_mutex,
    std::atomic_bool& shutdown_requested)
{
    std::string request_message;
    std::string transport_error;

    if (receive_framed_message(
            client_socket,
            request_message,
            transport_error) == false)
    {
        std::cerr
            << "Failed to receive client request: "
            << transport_error
            << '\n';
        return false;
    }

    CommandRequest request;
    std::string parse_error;
    CommandResponse response;

    if (parse_command_request(
            request_message,
            request,
            parse_error))
    {
        std::ostringstream command_output;
        bool shell_should_continue = true;

        {
            std::lock_guard<std::mutex> lock(command_mutex);

            shell_should_continue =
                handle_command(
                    request.command,
                    relay,
                    command_output);
        }

        response = CommandResponse{
            request.request_id,
            true,
            command_output.str()
        };

        if (shell_should_continue == false)
        {
            shutdown_requested.store(true);

            // Wake the main thread if it is blocked in accept().
            shutdown(server_socket, SHUT_RDWR);
        }
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
        serialize_command_response(response);

    if (send_framed_message(
            client_socket,
            response_message,
            transport_error) == false)
    {
        std::cerr
            << "Failed to send server response: "
            << transport_error
            << '\n';
        return false;
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

    if (listen(server_socket, 8) < 0)
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
        << std::endl;

    SimulatedRelay relay;
    std::mutex command_mutex;
    std::atomic_bool shutdown_requested{false};
    std::vector<std::thread> client_threads;
    bool server_failed = false;

    while (shutdown_requested.load() == false)
    {
        const int client_socket =
            accept(server_socket, nullptr, nullptr);

        if (client_socket < 0)
        {
            if (shutdown_requested.load())
            {
                break;
            }

            if (errno == EINTR)
            {
                continue;
            }

            std::cerr
                << "Failed to accept client: "
                << std::strerror(errno)
                << '\n';

            server_failed = true;
            break;
        }

        if (shutdown_requested.load())
        {
            close(client_socket);
            break;
        }

        client_threads.emplace_back(
            [client_socket,
             server_socket,
             &relay,
             &command_mutex,
             &shutdown_requested]()
            {
                const bool handled =
                    handle_client(
                        client_socket,
                        server_socket,
                        relay,
                        command_mutex,
                        shutdown_requested);

                close(client_socket);

                if (handled == false)
                {
                    std::cerr
                        << "Failed to handle client request\n";
                }
            });
    }

    for (std::thread& client_thread : client_threads)
    {
        if (client_thread.joinable())
        {
            client_thread.join();
        }
    }

    close(server_socket);

    if (server_failed)
    {
        return 1;
    }

    std::cout << "Hardware test server stopped\n";
    return 0;
}
