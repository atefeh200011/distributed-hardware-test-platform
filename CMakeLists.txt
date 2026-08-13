cmake_minimum_required(VERSION 3.20)

project(hardware_test_platform VERSION 0.1.0 LANGUAGES CXX)
find_package(nlohmann_json 3.11 REQUIRED)

add_executable(hwtest
    src/command_shell.cpp
    src/main.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
    src/test_procedure_json.cpp
)

target_compile_features(hwtest PRIVATE cxx_std_20)


target_compile_options(hwtest PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(
    hwtest
    PRIVATE nlohmann_json::nlohmann_json
)

enable_testing()

add_executable(command_shell_tests
    tests/command_shell_tests.cpp
    src/command_shell.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
    src/test_procedure_json.cpp
)

target_compile_features(command_shell_tests PRIVATE cxx_std_20)

target_include_directories(command_shell_tests PRIVATE src)

target_link_libraries(
    command_shell_tests
    PRIVATE nlohmann_json::nlohmann_json
)

add_test(
    NAME command_shell_commands
    COMMAND command_shell_tests
)

add_executable(simulated_relay_tests
    tests/simulated_relay_tests.cpp
    src/simulated_relay.cpp
)

target_compile_features(simulated_relay_tests PRIVATE cxx_std_20)

target_include_directories(simulated_relay_tests PRIVATE src)

add_test(
    NAME simulated_relay
    COMMAND simulated_relay_tests
)

add_executable(test_procedure_tests
    tests/test_procedure_tests.cpp
)

target_compile_features(test_procedure_tests PRIVATE cxx_std_20)

target_compile_options(test_procedure_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(test_procedure_tests PRIVATE src)

add_test(
    NAME test_procedure_model
    COMMAND test_procedure_tests
)

add_executable(test_executor_tests
    tests/test_executor_tests.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
)

target_compile_features(test_executor_tests PRIVATE cxx_std_20)

target_compile_options(test_executor_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(test_executor_tests PRIVATE src)

add_test(
    NAME test_executor
    COMMAND test_executor_tests
)

add_executable(test_procedure_json_tests
    tests/test_procedure_json_tests.cpp
    src/test_procedure_json.cpp
)

target_compile_features(test_procedure_json_tests PRIVATE cxx_std_20)

target_compile_options(test_procedure_json_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(test_procedure_json_tests PRIVATE src)

target_link_libraries(
    test_procedure_json_tests
    PRIVATE nlohmann_json::nlohmann_json
)

add_test(
    NAME test_procedure_json
    COMMAND test_procedure_json_tests
)
