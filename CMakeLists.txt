cmake_minimum_required(VERSION 3.20)

project(hardware_test_platform VERSION 0.1.0 LANGUAGES CXX)
find_package(nlohmann_json 3.11 REQUIRED)
find_package(Threads REQUIRED)

add_executable(hwtest
    src/command_shell.cpp
    src/main.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
    src/test_procedure_json.cpp
    src/test_report_json.cpp
    src/relay_registry.cpp
    src/device_configuration_json.cpp
    src/device_factory.cpp
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
    src/test_report_json.cpp
    src/relay_registry.cpp
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
    src/relay_registry.cpp
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
add_executable(test_executor_registry_tests
    tests/test_executor_registry_tests.cpp
    src/test_executor.cpp
    src/relay_registry.cpp
    src/simulated_relay.cpp
)

target_compile_features(
    test_executor_registry_tests
    PRIVATE cxx_std_20
)

target_compile_options(
    test_executor_registry_tests
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(
    test_executor_registry_tests
    PRIVATE src
)

add_test(
    NAME test_executor_registry
    COMMAND test_executor_registry_tests
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
add_executable(test_procedure_device_tests
    tests/test_procedure_device_tests.cpp
    src/test_procedure_json.cpp
)

target_compile_features(
    test_procedure_device_tests
    PRIVATE cxx_std_20
)

target_compile_options(
    test_procedure_device_tests
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(
    test_procedure_device_tests
    PRIVATE src
)

target_link_libraries(
    test_procedure_device_tests
    PRIVATE nlohmann_json::nlohmann_json
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

target_compile_features(
    relay_registry_tests
    PRIVATE cxx_std_20
)

target_compile_options(
    relay_registry_tests
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(
    relay_registry_tests
    PRIVATE src
)

add_test(
    NAME relay_registry
    COMMAND relay_registry_tests
)

add_executable(network_protocol_tests
    tests/network_protocol_tests.cpp
    src/network_protocol_json.cpp
)

target_compile_features(network_protocol_tests PRIVATE cxx_std_20)

target_compile_options(network_protocol_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(network_protocol_tests PRIVATE src)

target_link_libraries(
    network_protocol_tests
    PRIVATE nlohmann_json::nlohmann_json
)

add_test(
    NAME network_protocol
    COMMAND network_protocol_tests
)

add_executable(network_framing_tests
    tests/network_framing_tests.cpp
    src/network_framing.cpp
)

target_compile_features(network_framing_tests PRIVATE cxx_std_20)

target_compile_options(network_framing_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(network_framing_tests PRIVATE src)

add_test(
    NAME network_framing
    COMMAND network_framing_tests
)
add_executable(hwtest_server
    src/tcp_server_main.cpp
    src/command_shell.cpp
    src/network_framing.cpp
    src/network_protocol_json.cpp
    src/simulated_relay.cpp
    src/test_executor.cpp
    src/test_procedure_json.cpp
    src/tcp_transport.cpp
    src/logger.cpp
    src/test_report_json.cpp
    src/relay_registry.cpp
)

target_compile_features(hwtest_server PRIVATE cxx_std_20)

target_compile_options(hwtest_server PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(hwtest_server PRIVATE src)

target_link_libraries(
    hwtest_server
    PRIVATE
        nlohmann_json::nlohmann_json
        Threads::Threads
)

add_executable(hwtest_client
    src/tcp_client_main.cpp
    src/network_framing.cpp
    src/network_protocol_json.cpp
    src/tcp_transport.cpp
)

target_compile_features(hwtest_client PRIVATE cxx_std_20)

target_compile_options(hwtest_client PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(hwtest_client PRIVATE src)

target_link_libraries(
    hwtest_client
    PRIVATE nlohmann_json::nlohmann_json
)
add_test(
    NAME tcp_client_server_integration
    COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/tcp_integration_test.sh
        $<TARGET_FILE:hwtest_server>
        $<TARGET_FILE:hwtest_client>
        ${CMAKE_CURRENT_SOURCE_DIR}/procedures/relay_smoke_test.json
        ${CMAKE_CURRENT_SOURCE_DIR}/procedures/multi_relay_test.json
)

set_tests_properties(
    tcp_client_server_integration
    PROPERTIES
        TIMEOUT 10
        RUN_SERIAL TRUE
)
add_executable(tcp_transport_tests
    tests/tcp_transport_tests.cpp
    src/tcp_transport.cpp
    src/network_framing.cpp
)

target_compile_features(tcp_transport_tests PRIVATE cxx_std_20)

target_compile_options(tcp_transport_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(tcp_transport_tests PRIVATE src)

add_test(
    NAME tcp_transport
    COMMAND tcp_transport_tests
)
add_executable(device_configuration_tests
    tests/device_configuration_tests.cpp
)

target_compile_features(
    device_configuration_tests
    PRIVATE cxx_std_20
)

target_compile_options(
    device_configuration_tests
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(
    device_configuration_tests
    PRIVATE src
)

add_test(
    NAME device_configuration
    COMMAND device_configuration_tests
)

add_executable(test_result_tests
    tests/test_result_tests.cpp
)

target_compile_features(test_result_tests PRIVATE cxx_std_20)

target_compile_options(test_result_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(test_result_tests PRIVATE src)

add_test(
    NAME test_result_model
    COMMAND test_result_tests
)
add_executable(logger_tests
    tests/logger_tests.cpp
    src/logger.cpp
)

target_compile_features(logger_tests PRIVATE cxx_std_20)

target_compile_options(logger_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(logger_tests PRIVATE src)

target_link_libraries(
    logger_tests
    PRIVATE Threads::Threads
)

add_test(
    NAME logger
    COMMAND logger_tests
)
add_executable(test_report_json_tests
    tests/test_report_json_tests.cpp
    src/test_report_json.cpp
)

target_compile_features(test_report_json_tests PRIVATE cxx_std_20)

target_compile_options(test_report_json_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(test_report_json_tests PRIVATE src)

target_link_libraries(
    test_report_json_tests
    PRIVATE nlohmann_json::nlohmann_json
)

add_test(
    NAME test_report_json
    COMMAND test_report_json_tests
)
add_executable(device_configuration_json_tests
    tests/device_configuration_json_tests.cpp
    src/device_configuration_json.cpp
)

target_compile_features(
    device_configuration_json_tests
    PRIVATE cxx_std_20
)

target_compile_options(
    device_configuration_json_tests
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(
    device_configuration_json_tests
    PRIVATE src
)

target_link_libraries(
    device_configuration_json_tests
    PRIVATE nlohmann_json::nlohmann_json
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

target_compile_features(
    device_factory_tests
    PRIVATE cxx_std_20
)

target_compile_options(
    device_factory_tests
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(
    device_factory_tests
    PRIVATE src
)

add_test(
    NAME device_factory
    COMMAND device_factory_tests
)
