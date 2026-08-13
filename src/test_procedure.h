#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class TestAction
{
    relay_on,
    relay_off,
    expect_relay_on,
    expect_relay_off
};

struct TestStep
{
    std::string name;
    TestAction action;
    std::size_t retries{0};
};

struct TestProcedure
{
    std::string name;
    std::vector<TestStep> steps;
};
