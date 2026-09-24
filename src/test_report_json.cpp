#include "test_report_json.h"

#include <string>

#include <nlohmann/json.hpp>

namespace
{
std::string result_status(const TestResult& result)
{
    if (result.cancelled)
    {
        return "CANCELLED";
    }

    if (result.passed)
    {
        return "PASS";
    }

    return "FAIL";
}

std::string step_status(const StepResult& step)
{
    if (step.passed)
    {
        return "PASS";
    }

    return "FAIL";
}
}

std::string serialize_test_report(
    const TestResult& result)
{
    nlohmann::json report{
        {"procedure", result.procedure_name},
        {"status", result_status(result)},
        {"completed_steps", result.completed_steps},
        {"duration_ms", result.duration_ms},
        {"message", result.message},
        {"steps", nlohmann::json::array()}
    };

    if (result.failed_step.has_value())
    {
        report["failed_step"] =
            result.failed_step.value();
    }
    else
    {
        report["failed_step"] = nullptr;
    }

    for (const StepResult& step : result.steps)
    {
        report["steps"].push_back(
            {
                {"name", step.name},
                {"status", step_status(step)},
                {"attempts", step.attempts},
                {"duration_ms", step.duration_ms},
                {"message", step.message}
            });
    }

    return report.dump(4);
}
