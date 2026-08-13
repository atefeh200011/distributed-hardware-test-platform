#pragma once

#include <ostream>

#include "relay.h"
#include "test_procedure.h"
#include "test_result.h"

TestResult execute_procedure(
    const TestProcedure& procedure,
    IRelay& relay,
    std::ostream& output);