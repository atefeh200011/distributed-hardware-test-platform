#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct StepResult
{
    std::string name;
    bool passed;
    std::size_t attempts;
    std::int64_t duration_ms;
    std::string message;
};

struct TestResult
{
    bool passed;
    std::size_t completed_steps;
    std::optional<std::string> failed_step;
    std::string message;
    bool cancelled{false};

    std::string procedure_name{};
    std::int64_t duration_ms{0};
    std::vector<StepResult> steps{};
};
