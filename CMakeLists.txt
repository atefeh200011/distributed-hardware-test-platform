cmake_minimum_required(VERSION 3.20)

project(
    hardware_test_platform
    VERSION 1.0.0
    LANGUAGES CXX
)
configure_file(
    src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
    @ONLY
)

option(
    HWTEST_ENABLE_SANITIZERS
    "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
    OFF
)

find_package(nlohmann_json 3.11 REQUIRED)
find_package(Threads REQUIRED)

add_library(hwtest_project_options INTERFACE)

target_compile_features(
    hwtest_project_options
    INTERFACE cxx_std_20
)

target_include_directories(
    hwtest_project_options
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/generated
)

if(MSVC)
    target_compile_options(
        hwtest_project_options
        INTERFACE
            /W4
            /permissive-
    )
else()
    target_compile_options(
        hwtest_project_options
        INTERFACE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

if(HWTEST_ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            hwtest_project_options
            INTERFACE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )

        target_link_options(
            hwtest_project_options
            INTERFACE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )
    else()
        message(
            FATAL_ERROR
            "HWTEST_ENABLE_SANITIZERS requires GCC or Clang"
        )
    endif()
endif()

enable_testing()

add_executable(hwtest
    src/command_shell.cpp
    src/device_configuration_json.cpp
    src/device_factory.cpp
    src/main.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
    src/test_procedure_json.cpp
    src/test_report_json.cpp
)

target_link_libraries(
    hwtest
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_executable(hwtest_server
    src/command_shell.cpp
    src/device_configuration_json.cpp
    src/device_factory.cpp
    src/logger.cpp
    src/network_framing.cpp
    src/network_protocol_json.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
    src/tcp_server_main.cpp
    src/tcp_transport.cpp
    src/test_executor.cpp
    src/test_procedure_json.cpp
    src/test_report_json.cpp
)

target_link_libraries(
    hwtest_server
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
        Threads::Threads
)

add_executable(hwtest_client
    src/network_framing.cpp
    src/network_protocol_json.cpp
    src/tcp_client_main.cpp
    src/tcp_transport.cpp
)

target_link_libraries(
    hwtest_client
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_executable(command_shell_tests
    tests/command_shell_tests.cpp
    src/command_shell.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
    src/test_procedure_json.cpp
    src/test_report_json.cpp
)

target_link_libraries(
    command_shell_tests
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_test(
    NAME command_shell_commands
    COMMAND command_shell_tests
)

add_executable(simulated_relay_tests
    tests/simulated_relay_tests.cpp
    src/simulated_relay.cpp
)

target_link_libraries(
    simulated_relay_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME simulated_relay
    COMMAND simulated_relay_tests
)

add_executable(test_procedure_tests
    tests/test_procedure_tests.cpp
)

target_link_libraries(
    test_procedure_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME test_procedure_model
    COMMAND test_procedure_tests
)

add_executable(test_executor_tests
    tests/test_executor_tests.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
)

target_link_libraries(
    test_executor_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME test_executor
    COMMAND test_executor_tests
)

add_executable(test_executor_registry_tests
    tests/test_executor_registry_tests.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
)

target_link_libraries(
    test_executor_registry_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME test_executor_registry
    COMMAND test_executor_registry_tests
)

add_executable(test_procedure_json_tests
    tests/test_procedure_json_tests.cpp
    src/test_procedure_json.cpp
)

target_link_libraries(
    test_procedure_json_tests
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_test(
    NAME test_procedure_json
    COMMAND test_procedure_json_tests
)

add_executable(test_procedure_device_tests
    tests/test_procedure_device_tests.cpp
    src/test_procedure_json.cpp
)

target_link_libraries(
    test_procedure_device_tests
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_test(
    NAME test_procedure_device
    COMMAND test_procedure_device_tests
)

add_executable(relay_registry_tests
    tests/relay_registry_tests.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
)

target_link_libraries(
    relay_registry_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME relay_registry
    COMMAND relay_registry_tests
)

add_executable(network_protocol_tests
    tests/network_protocol_tests.cpp
    src/network_protocol_json.cpp
)

target_link_libraries(
    network_protocol_tests
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_test(
    NAME network_protocol
    COMMAND network_protocol_tests
)

add_executable(network_framing_tests
    tests/network_framing_tests.cpp
    src/network_framing.cpp
)

target_link_libraries(
    network_framing_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME network_framing
    COMMAND network_framing_tests
)

add_executable(tcp_transport_tests
    tests/tcp_transport_tests.cpp
    src/network_framing.cpp
    src/tcp_transport.cpp
)

target_link_libraries(
    tcp_transport_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME tcp_transport
    COMMAND tcp_transport_tests
)

add_executable(device_configuration_tests
    tests/device_configuration_tests.cpp
)

target_link_libraries(
    device_configuration_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME device_configuration
    COMMAND device_configuration_tests
)

add_executable(test_result_tests
    tests/test_result_tests.cpp
)

target_link_libraries(
    test_result_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME test_result_model
    COMMAND test_result_tests
)

add_executable(logger_tests
    tests/logger_tests.cpp
    src/logger.cpp
)

target_link_libraries(
    logger_tests
    PRIVATE
        hwtest_project_options
        Threads::Threads
)

add_test(
    NAME logger
    COMMAND logger_tests
)

add_executable(test_report_json_tests
    tests/test_report_json_tests.cpp
    src/test_report_json.cpp
)

target_link_libraries(
    test_report_json_tests
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_test(
    NAME test_report_json
    COMMAND test_report_json_tests
)

add_executable(device_configuration_json_tests
    tests/device_configuration_json_tests.cpp
    src/device_configuration_json.cpp
)

target_link_libraries(
    device_configuration_json_tests
    PRIVATE
        hwtest_project_options
        nlohmann_json::nlohmann_json
)

add_test(
    NAME device_configuration_json
    COMMAND device_configuration_json_tests
)

add_executable(device_factory_tests
    tests/device_factory_tests.cpp
    src/device_factory.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
)

target_link_libraries(
    device_factory_tests
    PRIVATE hwtest_project_options
)

add_test(
    NAME device_factory
    COMMAND device_factory_tests
)

add_test(
    NAME tcp_client_server_integration
    COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/tcp_integration_test.sh
        $<TARGET_FILE:hwtest_server>
        $<TARGET_FILE:hwtest_client>
        ${CMAKE_CURRENT_SOURCE_DIR}/procedures/relay_smoke_test.json
        ${CMAKE_CURRENT_SOURCE_DIR}/procedures/multi_relay_test.json
        ${CMAKE_CURRENT_SOURCE_DIR}/config/test_devices.json
)

set_tests_properties(
    tcp_client_server_integration
    PROPERTIES
        TIMEOUT 10
        RUN_SERIAL TRUE
)

add_test(
    NAME configuration_startup
    COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/configuration_startup_test.sh
        $<TARGET_FILE:hwtest>
        $<TARGET_FILE:hwtest_server>
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/data/invalid_devices.json
)

set_tests_properties(
    configuration_startup
    PROPERTIES
        TIMEOUT 10
        RUN_SERIAL TRUE
)