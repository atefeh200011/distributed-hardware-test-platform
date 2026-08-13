#pragma once

#include <ostream>

#include "relay.h"
#include "test_procedure.h"

bool execute_procedure(
    const TestProcedure& procedure,
    IRelay& relay,
    std::ostream& output);
    