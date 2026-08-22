#include "tcp_transport.h"

#include <cerrno>
#include <cstring>
#include <string>

#include <sys/socket.h>

#include "network_framing.h"

namespace
{
constexpr std::size_t maximum_message_size = 64 * 1024;
}

bool send_framed_message(
    int socket_descriptor,
    const std::string& message,
    std::string& error_message)
{
    const std::string framed_message =
        frame_message(message);

    std::size_t total_sent = 0;

    while (total_sent < framed_message.size())
    {
        const ssize_t sent = send(
            socket_descriptor,
            framed_message.data() + total_sent,
            framed_message.size() - total_sent,
            MSG_NOSIGNAL);

        if (sent < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            error_message =
                "send failed: " +
                std::string(std::strerror(errno));
            return false;
        }

        if (sent == 0)
        {
            error_message =
                "send returned zero before message completed";
            return false;
        }

        total_sent += static_cast<std::size_t>(sent);
    }

    error_message.clear();
    return true;
}

bool receive_framed_message(
    int socket_descriptor,
    std::string& message,
    std::string& error_message)
{
    std::string receive_buffer;
    char received_data[4096];

    while (extract_next_message(
               receive_buffer,
               message) == false)
    {
        const ssize_t received = recv(
            socket_descriptor,
            received_data,
            sizeof(received_data),
            0);

        if (received == 0)
        {
            error_message =
                "peer disconnected before completing message";
            return false;
        }

        if (received < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            error_message =
                "receive failed: " +
                std::string(std::strerror(errno));
            return false;
        }

        receive_buffer.append(
            received_data,
            static_cast<std::size_t>(received));

        if (receive_buffer.size() > maximum_message_size)
        {
            error_message =
                "received message exceeds maximum size";
            return false;
        }
    }

    error_message.clear();
    return true;
}
