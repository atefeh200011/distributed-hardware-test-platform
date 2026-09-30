# Distributed Hardware Test and Control Platform

[![CI](https://github.com/atefeh200011/distributed-hardware-test-platform/actions/workflows/ci.yml/badge.svg)](https://github.com/atefeh200011/distributed-hardware-test-platform/actions/workflows/ci.yml)

A modern C++20 platform for deterministic hardware testing,
configuration-driven device control, network communication, automated test
execution, structured logging, and machine-readable test reports.

## Project status

Version `1.0.0` complete.

The platform supports configuration-driven simulated devices, local and remote
device control, multi-device JSON test procedures, reliable execution,
structured logging, machine-readable reports, automated testing, sanitizer
verification, and continuous integration.

## Current capabilities

- Hardware abstraction through the `IRelay` interface
- Deterministic simulated relay drivers
- JSON-configured device creation
- Named relay registration and lookup
- Configurable initial device states
- Independent state for multiple relay devices
- Interactive command-line device control
- JSON-defined, device-aware test procedures
- Step retries and timeout detection
- Cooperative test cancellation
- Structured procedure and step results
- Machine-readable JSON test reports
- TCP client/server communication
- Concurrent TCP client handling
- Persistent device state across TCP connections
- JSON request and response messages
- Newline-delimited network framing
- Thread-safe persistent server logging
- Unit, integration, concurrency, and startup-validation tests
- AddressSanitizer and UndefinedBehaviorSanitizer support
- GitHub Actions continuous integration
- MIT open-source license

## Version 1.0.0 highlights

- Configuration-driven device creation
- Named multi-device control and independent device state
- Local command-line and concurrent TCP client/server applications
- JSON-defined, device-aware test procedures
- Retries, timeout detection, and cooperative cancellation
- Structured execution results and JSON report generation
- Thread-safe persistent server logging
- Automated unit and integration testing
- Sanitized Debug builds
- GitHub Actions continuous integration
- MIT License

See [CHANGELOG.md](CHANGELOG.md) for the complete release history.

## Motivation

I built this project to develop practical skills in:

- Modern C++20
- Hardware abstraction
- Configuration management
- JSON parsing and validation
- TCP networking
- Concurrent server design
- Automated hardware testing
- Reliable test execution
- Structured logging and reporting
- CMake and continuous integration
- Maintainable software architecture

## Requirements

- Ubuntu or Ubuntu on WSL2
- GCC 13 or newer
- CMake 3.20 or newer
- Ninja
- Git
- nlohmann/json 3.11 or newer
- POSIX sockets and threads

Install the required Ubuntu packages:

~~~bash
sudo apt update
sudo apt install build-essential cmake ninja-build git nlohmann-json3-dev
~~~

## Build

Configure the project with CMake and Ninja:

~~~bash
cmake -S . -B build -G Ninja
~~~

Compile the applications and tests:

~~~bash
cmake --build build
~~~

For a clean rebuild:

~~~bash
cmake --build build --clean-first
~~~

### Sanitized build

Configure a separate Debug build with AddressSanitizer and
UndefinedBehaviorSanitizer enabled:

~~~bash
cmake \
    -S . \
    -B build-sanitized \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DHWTEST_ENABLE_SANITIZERS=ON

cmake --build build-sanitized
ctest --test-dir build-sanitized --output-on-failure
~~~

## Built applications

The build produces three primary executables:

| Executable | Purpose |
| --- | --- |
| `hwtest` | Interactive local hardware-control application |
| `hwtest_server` | Concurrent TCP hardware-control server |
| `hwtest_client` | Command-line TCP client |

## Device configuration

Devices are created at startup from a JSON configuration file.

The default configuration is:

~~~text
config/devices.json
~~~

Example configuration:

~~~json
{
    "devices": [
        {
            "name": "relay-1",
            "type": "simulated_relay",
            "initial_state": "off"
        },
        {
            "name": "relay-2",
            "type": "simulated_relay",
            "initial_state": "on"
        }
    ]
}
~~~

Each device requires:

| Field | Description |
| --- | --- |
| `name` | Unique device name used by commands and procedures |
| `type` | Device implementation type |
| `initial_state` | Initial relay state: `on` or `off` |

The current supported device type is:

~~~text
simulated_relay
~~~

Configuration validation rejects invalid input such as:

- Missing required fields
- Duplicate device names
- Unsupported device types
- Invalid initial states
- Empty device names
- Invalid JSON

## Local command-line application

Start the local application with the default configuration:

~~~bash
./build/hwtest
~~~

You may also provide a configuration file explicitly:

~~~bash
./build/hwtest config/devices.json
~~~

Example startup:

~~~text
Hardware Test Platform version 1.0.0
Loaded 2 configured devices from config/devices.json
hwtest>
~~~

### Available commands

| Command | Description |
| --- | --- |
| `help` | Show available commands |
| `status` | Show platform status |
| `relays` | List registered relays |
| `relay <name> on` | Switch a named relay on |
| `relay <name> off` | Switch a named relay off |
| `relay <name> status` | Show a named relay’s state |
| `run <file>` | Run a JSON test procedure |
| `exit` | Exit the application |

Command whitespace is normalized, so leading, trailing, and repeated spaces are
accepted.

Example session:

~~~text
Hardware Test Platform version 1.0.0
Loaded 2 configured devices from config/devices.json
hwtest> relays
Available relays:
  relay-1
  relay-2
hwtest> relay relay-1 on
Relay relay-1 state: on
hwtest> relay relay-1 status
Relay relay-1 state: on
hwtest> relay relay-2 status
Relay relay-2 state: on
hwtest> relay relay-1 off
Relay relay-1 state: off
hwtest> exit
Shutting down the hardware test platform project.
~~~

## Device registry and factory

The device factory creates configured relay implementations from validated JSON
configuration.

The relay registry:

- Owns the configured relay instances
- Maps unique names to relay objects
- Supports deterministic name lookup
- Allows commands and procedures to select devices
- Preserves independent state for each device

The command shell and test executor depend on the `IRelay` interface rather
than directly depending on `SimulatedRelay`.

Future physical relay implementations can implement the same interface without
requiring changes to the command-processing or procedure-execution logic.

## JSON test procedures

Test procedures contain an ordered sequence of hardware actions and
expectations.

Run the single-relay smoke test:

~~~text
run procedures/relay_smoke_test.json
~~~

Run the multi-relay test:

~~~text
run procedures/multi_relay_test.json
~~~

Example procedure:

~~~json
{
    "name": "Relay smoke test",
    "steps": [
        {
            "name": "Switch relay on",
            "device": "relay-1",
            "action": "relay_on",
            "retries": 0,
            "timeout_ms": 1000
        },
        {
            "name": "Verify relay on",
            "device": "relay-1",
            "action": "expect_relay_on",
            "retries": 2,
            "timeout_ms": 500
        },
        {
            "name": "Switch relay off",
            "device": "relay-1",
            "action": "relay_off",
            "retries": 0,
            "timeout_ms": 1000
        },
        {
            "name": "Verify relay off",
            "device": "relay-1",
            "action": "expect_relay_off",
            "retries": 0,
            "timeout_ms": 1000
        }
    ]
}
~~~

### Procedure fields

A procedure contains:

| Field | Description |
| --- | --- |
| `name` | Human-readable procedure name |
| `steps` | Ordered collection of test steps |

A step contains:

| Field | Description |
| --- | --- |
| `name` | Human-readable step name |
| `device` | Target registered device |
| `action` | Hardware action or expectation |
| `retries` | Additional attempts after the first failure |
| `timeout_ms` | Maximum permitted step duration in milliseconds |

If `device` is omitted, it defaults to `relay-1`.

If `retries` is omitted, it defaults to `0`.

If `timeout_ms` is omitted, it defaults to `1000`.

### Supported actions

| Action | Behavior |
| --- | --- |
| `relay_on` | Switch the selected relay on |
| `relay_off` | Switch the selected relay off |
| `expect_relay_on` | Fail if the selected relay is off |
| `expect_relay_off` | Fail if the selected relay is on |

## Reliable test execution

The executor provides:

- Ordered step execution
- Named-device resolution
- Retry handling
- Timeout detection
- Cooperative cancellation
- Completed-step accounting
- Failed-step identification
- Per-step attempt counts
- Per-step execution durations
- Overall procedure duration
- Structured pass, failure, and cancellation results

A retry count represents additional attempts. For example:

~~~json
"retries": 2
~~~

allows up to three total attempts.

## JSON test reports

Every executed procedure produces a JSON report in the `reports/` directory.

Example shell output:

~~~text
Running procedure: Relay smoke test
Step: Switch relay on
Step: Verify relay on
Step: Switch relay off
Step: Verify relay off
Result: PASS
Report written: reports/relay_smoke_test-1790264830684.json
~~~

Example report:

~~~json
{
    "completed_steps": 4,
    "duration_ms": 0,
    "failed_step": null,
    "message": "All steps passed",
    "procedure": "Relay smoke test",
    "status": "PASS",
    "steps": [
        {
            "attempts": 1,
            "duration_ms": 0,
            "message": "Step passed",
            "name": "Switch relay on",
            "status": "PASS"
        }
    ]
}
~~~

Report files are runtime artifacts and are excluded from Git.

## Structured logging

The TCP server uses a thread-safe logger.

Log records include:

- UTC timestamp
- Log level
- Message

Example:

~~~text
2026-09-24T10:59:55Z [INFO] Hardware test server started
2026-09-24T10:59:55Z [INFO] Executing command: relay relay-1 status
2026-09-24T10:59:55Z [INFO] Hardware test server stopped
~~~

Log files are runtime artifacts and are excluded from Git.

## TCP client and server

Start the server with the default device configuration:

~~~bash
./build/hwtest_server
~~~

Or provide a configuration file:

~~~bash
./build/hwtest_server config/devices.json
~~~

The server listens on:

~~~text
127.0.0.1:5050
~~~

From another terminal, send commands with the client:

~~~bash
./build/hwtest_client relays
./build/hwtest_client relay relay-1 on
./build/hwtest_client relay relay-1 status
./build/hwtest_client run procedures/multi_relay_test.json
./build/hwtest_client exit
~~~

The server:

- Loads configured devices at startup
- Preserves device state across connections
- Accepts concurrent clients
- Executes commands through the shared command shell
- Returns structured JSON responses
- Stops when it receives the `exit` command

## Network protocol

The client and server exchange newline-delimited JSON messages.

Example request:

~~~json
{
    "request_id": "client-request-001",
    "command": "relay relay-1 status"
}
~~~

Example response:

~~~json
{
    "request_id": "client-request-001",
    "success": true,
    "output": "Relay relay-1 state: off\n"
}
~~~

Each JSON message is followed by a newline delimiter so the receiver can detect
the end of the message over the TCP byte stream.

## Test

Run all tests:

~~~bash
ctest --test-dir build --output-on-failure
~~~

The test suite covers:

- Command parsing and whitespace normalization
- Simulated relay behavior
- Test-procedure models
- JSON procedure parsing and validation
- Named-device parsing
- Retry, timeout, and cancellation behavior
- Multi-device execution
- Relay registry behavior
- Network request and response serialization
- Network framing
- Shared TCP transport
- Concurrent TCP client/server integration
- Remote procedure execution
- Device configuration models
- JSON device-configuration validation
- Device factory behavior
- Invalid startup configuration
- Structured result models
- Thread-safe logging
- JSON test-report generation

The current release contains 19 automated tests.

## Continuous integration

GitHub Actions runs the complete build and test suite for pushes to `main` and
for pull requests.

The CI matrix verifies:

- A normal Debug build
- A Debug build with AddressSanitizer and UndefinedBehaviorSanitizer enabled

The workflow is defined in:

~~~text
.github/workflows/ci.yml
~~~

## Architecture

### Local execution

~~~text
JSON device configuration
          |
          v
Device configuration parser
          |
          v
Device factory
          |
          v
Configured RelayRegistry
          |
          v
Command shell
          |
          v
Test executor
          |
          v
Structured TestResult
          |
          v
JSON report
~~~

### Distributed command execution

~~~text
TCP client
    |
    v
JSON request
    |
    v
TCP framing and transport
    |
    v
Concurrent TCP server
    |
    v
Command shell
    |
    v
Configured RelayRegistry
~~~

## Project structure

~~~text
.
├── .github/
│   └── workflows/
│       └── ci.yml
├── config/
│   ├── devices.json
│   └── test_devices.json
├── procedures/
│   ├── multi_relay_test.json
│   └── relay_smoke_test.json
├── src/
│   ├── command_shell.cpp
│   ├── command_shell.h
│   ├── device_configuration.h
│   ├── device_configuration_json.cpp
│   ├── device_configuration_json.h
│   ├── device_factory.cpp
│   ├── device_factory.h
│   ├── logger.cpp
│   ├── logger.h
│   ├── main.cpp
│   ├── network_framing.cpp
│   ├── network_framing.h
│   ├── network_protocol.h
│   ├── network_protocol_json.cpp
│   ├── network_protocol_json.h
│   ├── relay.h
│   ├── relay_registry.cpp
│   ├── relay_registry.h
│   ├── simulated_relay.cpp
│   ├── simulated_relay.h
│   ├── tcp_client_main.cpp
│   ├── tcp_server_main.cpp
│   ├── tcp_transport.cpp
│   ├── tcp_transport.h
│   ├── test_executor.cpp
│   ├── test_executor.h
│   ├── test_procedure.h
│   ├── test_procedure_json.cpp
│   ├── test_procedure_json.h
│   ├── test_report_json.cpp
│   ├── test_report_json.h
│   ├── test_result.h
│   └── version.h.in
├── tests/
│   ├── data/
│   │   └── invalid_devices.json
│   ├── command_shell_tests.cpp
│   ├── configuration_startup_test.sh
│   ├── device_configuration_json_tests.cpp
│   ├── device_configuration_tests.cpp
│   ├── device_factory_tests.cpp
│   ├── logger_tests.cpp
│   ├── network_framing_tests.cpp
│   ├── network_protocol_tests.cpp
│   ├── relay_registry_tests.cpp
│   ├── simulated_relay_tests.cpp
│   ├── tcp_integration_test.sh
│   ├── tcp_transport_tests.cpp
│   ├── test_executor_registry_tests.cpp
│   ├── test_executor_tests.cpp
│   ├── test_procedure_device_tests.cpp
│   ├── test_procedure_json_tests.cpp
│   ├── test_procedure_tests.cpp
│   ├── test_report_json_tests.cpp
│   └── test_result_tests.cpp
├── .gitignore
├── CHANGELOG.md
├── CMakeLists.txt
├── LICENSE
└── README.md
~~~

## Milestones

- Milestone 1: Project foundation and interactive command shell
- Milestone 2: Hardware abstraction and simulated relay
- Milestone 3: JSON-defined test procedures
- Milestone 4: Reliable execution, retries, timeouts, and cancellation
- Milestone 5: Concurrent TCP client/server communication
- Milestone 6: Structured logging and machine-readable reports
- Milestone 7: Named multi-device registration, control, and execution
- Milestone 8: Configuration-driven device creation and startup
- Milestone 9: Production polish, CI, documentation, and `v1.0.0` release

## License

This project is available under the [MIT License](LICENSE).

Copyright (c) 2026 Atefeh Mohammadpour.