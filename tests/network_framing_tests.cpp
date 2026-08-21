#include <iostream>
#include <string>

#include "network_framing.h"

int main()
{
    const std::string json_message =
        R"({"request_id":"request-001","command":"relay on"})";

    const std::string framed_message =
        frame_message(json_message);

    if (framed_message != json_message + '\n')
    {
        std::cerr << "FAIL: message framing is incorrect\n";
        return 1;
    }

    std::string partial_buffer =
        R"({"request_id":"request-001")";

    std::string extracted_message;

    if (extract_next_message(
            partial_buffer,
            extracted_message))
    {
        std::cerr
            << "FAIL: partial message should not be extracted\n";
        return 1;
    }

    partial_buffer +=
        R"(,"command":"relay on"})";
    partial_buffer += '\n';

    if (extract_next_message(
            partial_buffer,
            extracted_message) == false)
    {
        std::cerr
            << "FAIL: complete message should be extracted\n";
        return 1;
    }

    if (extracted_message != json_message)
    {
        std::cerr << "FAIL: extracted message is incorrect\n";
        return 1;
    }

    if (partial_buffer.empty() == false)
    {
        std::cerr
            << "FAIL: consumed message should leave empty buffer\n";
        return 1;
    }

    std::string multiple_messages =
        "first message\nsecond message\n";

    if (extract_next_message(
            multiple_messages,
            extracted_message) == false)
    {
        std::cerr << "FAIL: first message was not extracted\n";
        return 1;
    }

    if (extracted_message != "first message")
    {
        std::cerr << "FAIL: first message is incorrect\n";
        return 1;
    }

    if (multiple_messages != "second message\n")
    {
        std::cerr
            << "FAIL: second message should remain buffered\n";
        return 1;
    }

    if (extract_next_message(
            multiple_messages,
            extracted_message) == false)
    {
        std::cerr << "FAIL: second message was not extracted\n";
        return 1;
    }

    if (extracted_message != "second message")
    {
        std::cerr << "FAIL: second message is incorrect\n";
        return 1;
    }

    std::string windows_line = "windows message\r\n";

    if (extract_next_message(
            windows_line,
            extracted_message) == false)
    {
        std::cerr
            << "FAIL: Windows line ending was not extracted\n";
        return 1;
    }

    if (extracted_message != "windows message")
    {
        std::cerr
            << "FAIL: carriage return should be removed\n";
        return 1;
    }

    std::cout << "PASS: network message framing\n";
    return 0;
}
