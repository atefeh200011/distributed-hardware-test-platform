cmake_minimum_required(VERSION 3.20)

project(hardware_test_platform VERSION 0.1.0 LANGUAGES CXX)

add_executable(hwtest
    src/command_shell.cpp
    src/main.cpp
    src/simulated_relay.cpp
)

target_compile_features(hwtest PRIVATE cxx_std_20)

enable_testing()

add_executable(command_shell_tests
    tests/command_shell_tests.cpp
    src/command_shell.cpp
    src/simulated_relay.cpp
)

target_compile_features(command_shell_tests PRIVATE cxx_std_20)

target_include_directories(command_shell_tests PRIVATE src)

add_test(
    NAME command_shell_commands
    COMMAND command_shell_tests
)
add_executable(simulated_relay_tests
    tests/simulated_relay_tests.cpp
    src/simulated_relay.cpp
)
target_compile_features(simulated_relay_tests PRIVATE cxx_std_20)

target_compile_options(hwtest PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(simulated_relay_tests PRIVATE src)

add_test(
    NAME simulated_relay
    COMMAND simulated_relay_tests
)
