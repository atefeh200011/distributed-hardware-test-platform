#pragma once

#include <string>

#include "test_procedure.h"

bool parse_test_procedure_json(
    const std::string& json_text,
    TestProcedure& procedure,
    std::string& error_message);
    