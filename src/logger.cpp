#include "logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace
{
std::string level_to_string(LogLevel level)
{
    if (level == LogLevel::info)
    {
        return "INFO";
    }

    if (level == LogLevel::warning)
    {
        return "WARNING";
    }

    return "ERROR";
}

std::string current_utc_timestamp()
{
    const auto now =
        std::chrono::system_clock::now();

    const std::time_t current_time =
        std::chrono::system_clock::to_time_t(now);

    std::tm utc_time{};

    gmtime_r(&current_time, &utc_time);

    std::ostringstream timestamp;
    timestamp << std::put_time(
        &utc_time,
        "%Y-%m-%dT%H:%M:%SZ");

    return timestamp.str();
}
}

Logger::Logger(std::ostream& output)
    : output_(output)
{
}

void Logger::log(
    LogLevel level,
    const std::string& message)
{
    const std::string timestamp =
        current_utc_timestamp();

    const std::lock_guard<std::mutex> lock(mutex_);

    output_
        << timestamp
        << " ["
        << level_to_string(level)
        << "] "
        << message
        << '\n';
}

void Logger::info(const std::string& message)
{
    log(LogLevel::info, message);
}

void Logger::warning(const std::string& message)
{
    log(LogLevel::warning, message);
}

void Logger::error(const std::string& message)
{
    log(LogLevel::error, message);
}
