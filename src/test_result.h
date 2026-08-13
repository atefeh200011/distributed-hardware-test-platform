#pragma once

#include <cstddef>
#include <optional>
#include <string>

struct TestResult
{
    bool passed;
    std::size_t completed_steps;
    std::optional<std::string> failed_step;
    std::string message;
    bool cancelled{false};
};
