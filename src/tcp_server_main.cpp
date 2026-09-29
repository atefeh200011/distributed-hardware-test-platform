#include <arpa/inet.h>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "command_shell.h"
#include "logger.h"
#include "network_protocol.h"
#include "network_protocol_json.h"
#include "relay_registry.h"
#include "tcp_transport.h"
#include "device_configuration.h"
#include "device_configuration_json.h"
#include "device_factory.h"

namespace
{
constexpr int server_port = 5050;

bool handle_client(
    int client_socket,
    int server_socket,
    RelayRegistry& relays,
    std::mutex& command_mutex,
    std::atomic_bool& shutdown_requested,
    Logger& logger)
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

        logger.warning(
            "Failed to receive client request: " +
            transport_error);

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
        logger.info(
            "Executing command: " +
            request.command);

        std::ostringstream command_output;
        bool shell_should_continue = true;

        {
            std::lock_guard<std::mutex> lock(
                command_mutex);

            shell_should_continue =
                handle_command(
                    request.command,
                    relays,
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

            // Wake main() if it is blocked in accept().
            shutdown(server_socket, SHUT_RDWR);
        }
    }
    else
    {
        logger.warning(
            "Rejected invalid request: " +
            parse_error);

        response = CommandResponse{
            "unknown",
            false,
            "Invalid request: " +
                parse_error +
                '\n'
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

        logger.error(
            "Failed to send server response: " +
            transport_error);

        return false;
    }

    return true;
}
}

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr
            << "Usage: hwtest_server "
            << "[device-configuration-file]\n";
        return 1;
    }

    const std::string configuration_file = 
        argc == 2
            ? argv[1]
            : "config/devices.json";
    std::error_code directory_error;

    std::filesystem::create_directories(
        "logs",
        directory_error);

    if (directory_error)
    {
        std::cerr
            << "Failed to create log directory: "
            << directory_error.message()
            << '\n';
        return 1;
    }

    std::ofstream log_file(
        "logs/hwtest-server.log",
        std::ios::app);

    if (log_file.is_open() == false)
    {
        std::cerr
            << "Failed to open server log file\n";
        return 1;
    }

    Logger logger(log_file);
    logger.info("Hardware test server starting");

    PlatformConfiguration configuration;
    std::string configuration_error;

    if (load_device_configuration_file(
            configuration_file,
            configuration,
            configuration_error) == false)
    {
        const std::string error_message =
            "Failed to load device configuration: " +
            configuration_error;

        std::cerr << error_message << '\n';
        logger.error(error_message);
        return 1;
    }

    RelayRegistry relays;

    if (build_relay_registry(
            configuration,
            relays,
            configuration_error) == false)
    {
        const std::string error_message =
            "Failed to build device registry: " +
            configuration_error;

        std::cerr << error_message << '\n';
        logger.error(error_message);
        return 1;
    }

    logger.info(
        "Loaded " +
        std::to_string(relays.size()) +
        " configured devices from " +
        configuration_file);

    const int server_socket =
        socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        const std::string error_message =
            "Failed to create server socket: " +
            std::string(std::strerror(errno));

        std::cerr << error_message << '\n';
        logger.error(error_message);
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
        const std::string error_message =
            "Failed to configure server socket: " +
            std::string(std::strerror(errno));

        std::cerr << error_message << '\n';
        logger.error(error_message);

        close(server_socket);
        return 1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port =
        htons(server_port);

    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &server_address.sin_addr) != 1)
    {
        const std::string error_message =
            "Failed to configure server address";

        std::cerr << error_message << '\n';
        logger.error(error_message);

        close(server_socket);
        return 1;
    }

    if (bind(
            server_socket,
            reinterpret_cast<const sockaddr*>(
                &server_address),
            sizeof(server_address)) < 0)
    {
        const std::string error_message =
            "Failed to bind server to 127.0.0.1:" +
            std::to_string(server_port) +
            ": " +
            std::string(std::strerror(errno));

        std::cerr << error_message << '\n';
        logger.error(error_message);

        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 8) < 0)
    {
        const std::string error_message =
            "Failed to listen for clients: " +
            std::string(std::strerror(errno));

        std::cerr << error_message << '\n';
        logger.error(error_message);

        close(server_socket);
        return 1;
    }

    std::cout
        << "Hardware test server listening on "
        << "127.0.0.1:"
        << server_port
        << std::endl;

    logger.info(
        "TCP server listening on 127.0.0.1:" +
        std::to_string(server_port));

    std::mutex command_mutex;
    std::atomic_bool shutdown_requested{false};
    std::vector<std::thread> client_threads;
    bool server_failed = false;

    while (shutdown_requested.load() == false)
    {
        const int client_socket =
            accept(
                server_socket,
                nullptr,
                nullptr);

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

            const std::string error_message =
                "Failed to accept client: " +
                std::string(std::strerror(errno));

            std::cerr << error_message << '\n';
            logger.error(error_message);

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
             &relays,
             &command_mutex,
             &shutdown_requested,
             &logger]()
            {
                const bool handled =
                    handle_client(
                        client_socket,
                        server_socket,
                        relays,
                        command_mutex,
                        shutdown_requested,
                        logger);

                close(client_socket);

                if (handled == false)
                {
                    std::cerr
                        << "Failed to handle "
                        << "client request\n";

                    logger.error(
                        "Failed to handle "
                        "client request");
                }
            });
    }

    for (std::thread& client_thread :
         client_threads)
    {
        if (client_thread.joinable())
        {
            client_thread.join();
        }
    }

    close(server_socket);

    if (server_failed)
    {
        logger.error(
            "Hardware test server stopped "
            "because of an error");
        return 1;
    }

    logger.info("Hardware test server stopped");

    std::cout
        << "Hardware test server stopped\n";

    return 0;
}
