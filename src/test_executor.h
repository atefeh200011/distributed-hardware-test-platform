#pragma once

#include <atomic>
#include <ostream>

#include "relay.h"
#include "test_procedure.h"
#include "test_result.h"

TestResult execute_procedure(
    const TestProcedure& procedure,
    IRelay& relay,
    std::ostream& output,
    const std::atomic_bool* cancellation_requested = nullptr);
    