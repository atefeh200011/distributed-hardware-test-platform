#pragma once

#include <mutex>
#include <ostream>
#include <string>

enum class LogLevel
{
    info,
    warning,
    error
};

class Logger
{
public:
    explicit Logger(std::ostream& output);

    void log(
        LogLevel level,
        const std::string& message);

    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

private:
    std::ostream& output_;
    std::mutex mutex_;
};
