#pragma once

#include <string>

#include "test_result.h"

std::string serialize_test_report(
    const TestResult& result);