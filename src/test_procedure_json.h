#pragma once

#include <string>

#include "test_procedure.h"

bool parse_test_procedure_json(
    const std::string& json_text,
    TestProcedure& procedure,
    std::string& error_message);
 
bool load_test_procedure_file(
    const std::string& file_path,
    TestProcedure& procedure,
    std::string& error_message);
        