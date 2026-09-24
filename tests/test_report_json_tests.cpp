#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "test_report_json.h"

int main()
{
    const TestResult result{
        true,
        2,
        std::nullopt,
        "All steps passed",
        false,
        "Relay report test",
        12,
        {
            {
                "Switch relay on",
                true,
                1,
                5,
                "Step passed"
            },
            {
                "Verify relay on",
                true,
                2,
                7,
                "Step passed"
            }
        }
    };

    const std::string report_text =
        serialize_test_report(result);

    const nlohmann::json report =
        nlohmann::json::parse(report_text);

    if (report["procedure"] != "Relay report test")
    {
        std::cerr << "FAIL: report procedure is incorrect\n";
        return 1;
    }

    if (report["status"] != "PASS")
    {
        std::cerr << "FAIL: report status is incorrect\n";
        return 1;
    }

    if (report["completed_steps"] != 2)
    {
        std::cerr
            << "FAIL: report completed-step count is incorrect\n";
        return 1;
    }

    if (report["duration_ms"] != 12)
    {
        std::cerr << "FAIL: report duration is incorrect\n";
        return 1;
    }

    if (report["failed_step"].is_null() == false)
    {
        std::cerr
            << "FAIL: passing report should have no failed step\n";
        return 1;
    }

    if (report["steps"].size() != 2)
    {
        std::cerr << "FAIL: report should contain two steps\n";
        return 1;
    }

    if (report["steps"][1]["attempts"] != 2)
    {
        std::cerr
            << "FAIL: report attempt count is incorrect\n";
        return 1;
    }

    if (report["steps"][1]["status"] != "PASS")
    {
        std::cerr << "FAIL: step status is incorrect\n";
        return 1;
    }

    std::cout << "PASS: JSON test report serialization\n";
    return 0;
}
