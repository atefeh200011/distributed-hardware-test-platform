#pragma once

#include <string>

#include "test_result.h"

std::string serialize_test_report(
    const TestResult& result);

bool write_test_report_file(
    const std::string& file_path,
    const TestResult& result,
    std::string& error_message);    