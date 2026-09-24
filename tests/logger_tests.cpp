#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

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

    std::cout << "PASS: structured logger\n";
    return 0;
}
