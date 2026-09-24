#include "test_report_json.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

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

bool write_test_report_file(
    const std::string& file_path,
    const TestResult& result,
    std::string& error_message)
{
    const std::filesystem::path report_path(file_path);

    const std::filesystem::path parent_directory =
        report_path.parent_path();

    if (parent_directory.empty() == false)
    {
        std::error_code directory_error;

        std::filesystem::create_directories(
            parent_directory,
            directory_error);

        if (directory_error)
        {
            error_message =
                "could not create report directory: " +
                directory_error.message();
            return false;
        }
    }

    std::ofstream output_file(
        report_path,
        std::ios::trunc);

    if (output_file.is_open() == false)
    {
        error_message =
            "could not open report file: " + file_path;
        return false;
    }

    output_file
        << serialize_test_report(result)
        << '\n';

    if (output_file.good() == false)
    {
        error_message =
            "could not write report file: " + file_path;
        return false;
    }

    error_message.clear();
    return true;
}