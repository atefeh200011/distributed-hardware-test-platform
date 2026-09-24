#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "logger.h"

int main()
{
    std::ostringstream output;
    Logger logger(output);

    logger.info("Server started");
    logger.warning("Retrying test step");
    logger.error("Test step failed");

    const std::string log_text = output.str();

    if (log_text.find("[INFO] Server started\n") ==
        std::string::npos)
    {
        std::cerr << "FAIL: info log is incorrect\n";
        return 1;
    }

    if (log_text.find("[WARNING] Retrying test step\n") ==
        std::string::npos)
    {
        std::cerr << "FAIL: warning log is incorrect\n";
        return 1;
    }

    if (log_text.find("[ERROR] Test step failed\n") ==
        std::string::npos)
    {
        std::cerr << "FAIL: error log is incorrect\n";
        return 1;
    }

    const std::size_t line_count =
        static_cast<std::size_t>(
            std::count(
                log_text.begin(),
                log_text.end(),
                '\n'));

    if (line_count != 3)
    {
        std::cerr << "FAIL: logger should produce three lines\n";
        return 1;
    }

    if (log_text.size() < 20 ||
        log_text[4] != '-' ||
        log_text[7] != '-' ||
        log_text[10] != 'T')
    {
        std::cerr << "FAIL: timestamp format is incorrect\n";
        return 1;
    }
    std::ostringstream concurrent_output;
    Logger concurrent_logger(concurrent_output);

    constexpr int thread_count = 8;
    constexpr int messages_per_thread = 25;

    std::vector<std::thread> threads;

    for (int thread_index = 0;
        thread_index < thread_count;
        ++thread_index)
    {
        threads.emplace_back(
            [&concurrent_logger, thread_index]()
            {
                for (int message_index = 0;
                    message_index < messages_per_thread;
                    ++message_index)
                {
                    concurrent_logger.info(
                        "thread-" +
                        std::to_string(thread_index) +
                        "-message-" +
                        std::to_string(message_index));
                }
            });
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    const std::string concurrent_log_text =
        concurrent_output.str();

    const std::size_t concurrent_line_count =
        static_cast<std::size_t>(
            std::count(
                concurrent_log_text.begin(),
                concurrent_log_text.end(),
                '\n'));

    const std::size_t expected_line_count =
        static_cast<std::size_t>(
            thread_count * messages_per_thread);

    if (concurrent_line_count != expected_line_count)
    {
        std::cerr
            << "FAIL: concurrent logging lost or combined lines\n";
        return 1;
    }

    for (int thread_index = 0;
        thread_index < thread_count;
        ++thread_index)
    {
        for (int message_index = 0;
            message_index < messages_per_thread;
            ++message_index)
        {
            const std::string expected_message =
                "[INFO] thread-" +
                std::to_string(thread_index) +
                "-message-" +
                std::to_string(message_index) +
                '\n';

            if (concurrent_log_text.find(expected_message) ==
                std::string::npos)
            {
                std::cerr
                    << "FAIL: concurrent log message is missing\n";
                return 1;
            }
        }
    }
    std::cout << "PASS: structured logger\n";
    return 0;
}
