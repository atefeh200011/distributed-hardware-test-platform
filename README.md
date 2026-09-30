# Distributed Hardware Test and Control Platform

[![CI](https://github.com/atefeh200011/distributed-hardware-test-platform/actions/workflows/ci.yml/badge.svg)](https://github.com/atefeh200011/distributed-hardware-test-platform/actions/workflows/ci.yml)

A modern C++20 platform for deterministic hardware testing, configuration-driven
device control, network communication, automated test execution, structured
logging, and machine-readable test reports.

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
- Test execution with retries and timeout detection
- Cooperative test cancellation
- Structured procedure and step results
- TCP client/server communication
- Concurrent TCP client handling
- Persistent device state across connections
- JSON network request and response messages
- Thread-safe structured server logging
- Machine-readable JSON test reports
- Automated unit and integration tests

## Version 1.0.0 highlights

- Configuration-driven device creation
- Named multi-device control and independent device state
- Local command-line and concurrent TCP client/server applications
- JSON-defined, device-aware test procedures
- Retries, timeout detection, and cooperative cancellation
- Structured execution results and JSON report generation
- Thread-safe persistent server logging
- Unit, integration, concurrency, and startup-validation testing
- Optional AddressSanitizer and UndefinedBehaviorSanitizer builds
- GitHub Actions continuous integration
- MIT open-source license

See [CHANGELOG.md](CHANGELOG.md) for the complete release history.

## Motivation

I am building this project to develop practical skills in modern C++, hardware
abstraction, configuration management, networking, concurrency, automated
testing, and maintainable software architecture.

## Requirements

- Ubuntu on WSL2
- GCC 13 or newer
- CMake 3.20 or newer
- Ninja
- Git
- nlohmann/json 3.11 or newer
- POSIX sockets and threads

Install the required Ubuntu packages with:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build git nlohmann-json3-dev
```

## Build

Configure the project with CMake and Ninja:

```bash
cmake -S . -B build -G Ninja
```

Compile the applications and tests:

```bash
cmake --build build
```

For a clean rebuild:

```bash
cmake --build build --clean-first
```

### Sanitized build

Configure a separate Debug build with AddressSanitizer and
UndefinedBehaviorSanitizer enabled:

```bash
cmake \
    -S . \
    -B build-sanitized \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DHWTEST_ENABLE_SANITIZERS=ON

cmake --build build-sanitized
ctest --test-dir build-sanitized --output-on-failure
```

## Device configuration

Devices are created at startup from a JSON configuration file.

The default configuration is:

```text
config/devices.json
```

Example:

```json
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
```

Each device contains:

| Property | Required | Description |
| --- | --- | --- |
| `name` | Yes | Unique device name |
| `type` | Yes | Device implementation type |
| `initial_state` | No | Initial state: `on` or `off`; defaults to `off` |

Currently supported device types:

| Type | Description |
| --- | --- |
| `simulated_relay` | Deterministic in-memory relay implementation |

Configuration validation rejects:

- Missing or non-array `devices`
- Empty device lists
- Missing, empty, or invalid names
- Duplicate device names
- Missing or invalid device types
- Unsupported device types
- Invalid initial-state values
- Incorrect JSON value types
- Malformed JSON

The registry is built transactionally. If any device cannot be created, the
destination registry is not partially populated.

## Local command-line application

Start with the default configuration:

```bash
./build/hwtest
```

Start with an explicit configuration file:

```bash
./build/hwtest config/devices.json
```

Example startup:

```text
Hardware Test Platform version 0.1.0
Loaded 2 configured devices from config/devices.json
```

Available commands:

| Command | Description |
| --- | --- |
| `help` | Show available commands |
| `status` | Show platform status |
| `relays` | List registered relays |
| `relay <name> on` | Switch a named relay on |
| `relay <name> off` | Switch a named relay off |
| `relay <name> status` | Show a named relay state |
| `run <file>` | Load and execute a JSON test procedure |
| `exit` | Exit the application |

Example:

```text
hwtest> relays
Available relays:
  relay-1
  relay-2
hwtest> relay relay-1 status
Relay relay-1 state: off
hwtest> relay relay-2 status
Relay relay-2 state: on
hwtest> exit
Shutting down the hardware test platform project.
```

Invalid configuration prevents startup:

```bash
./build/hwtest config/missing.json
```

Example error:

```text
Failed to load device configuration: could not open device configuration file: config/missing.json
```

## Device registry and factory

The configuration pipeline is:

```text
devices.json
     |
     v
JSON configuration parser
     |
     v
PlatformConfiguration
     |
     v
Device factory
     |
     v
RelayRegistry
     |
     v
Command shell and test executor
```

The `RelayRegistry` owns relay implementations and provides:

- Registration by unique name
- Lookup by name
- Duplicate-name rejection
- Missing-device detection
- Sorted device listings
- Access through the `IRelay` abstraction

The device factory converts validated `DeviceConfiguration` objects into
initialized hardware implementations.

Future physical relay drivers can be added to the factory without changing the
command shell or test executor.

## TCP client and server

Start the server with the default configuration:

```bash
./build/hwtest_server
```

Start it with an explicit configuration:

```bash
./build/hwtest_server config/devices.json
```

The server listens on:

```text
127.0.0.1:5050
```

List configured devices from another terminal:

```bash
./build/hwtest_client relays
```

Control named relays remotely:

```bash
./build/hwtest_client relay relay-1 on
./build/hwtest_client relay relay-1 status
./build/hwtest_client relay relay-2 status
./build/hwtest_client relay relay-1 off
```

Run procedures remotely:

```bash
./build/hwtest_client run procedures/relay_smoke_test.json
./build/hwtest_client run procedures/multi_relay_test.json
```

Stop the server:

```bash
./build/hwtest_client exit
```

The server preserves device state between connections and handles multiple
clients concurrently.

## Network protocol

The client and server exchange newline-delimited JSON messages over TCP.

Example request:

```json
{
    "request_id": "client-request-001",
    "command": "relay relay-1 status"
}
```

Example response:

```json
{
    "request_id": "client-request-001",
    "success": true,
    "output": "Relay relay-1 state: off\n"
}
```

## JSON test procedures

Test procedures contain ordered device actions and expectations.

Each step may contain:

| Property | Required | Description |
| --- | --- | --- |
| `name` | Yes | Human-readable step name |
| `action` | Yes | Hardware action or expectation |
| `device` | No | Registered relay name; defaults to `relay-1` |
| `retries` | No | Additional attempts after the first failure |
| `timeout_ms` | No | Maximum permitted duration in milliseconds |

Supported actions:

| Action | Behavior |
| --- | --- |
| `relay_on` | Switch the selected relay on |
| `relay_off` | Switch the selected relay off |
| `expect_relay_on` | Fail if the selected relay is off |
| `expect_relay_off` | Fail if the selected relay is on |

Example step:

```json
{
    "name": "Verify second relay on",
    "device": "relay-2",
    "action": "expect_relay_on",
    "retries": 2,
    "timeout_ms": 500
}
```

Run the single-relay procedure:

```text
run procedures/relay_smoke_test.json
```

Run the multi-relay procedure:

```text
run procedures/multi_relay_test.json
```

Before each step, the executor finds the configured device in the registry. A
missing device fails with a structured diagnostic:

```text
Relay not found: missing
```

## Reliable test execution

The test executor supports:

- Named-device resolution
- Missing-device detection
- Retry handling
- Step timeout detection
- Cooperative cancellation
- Completed-step tracking
- Failed-step identification
- Procedure duration
- Per-step duration
- Attempt counts
- Diagnostic result messages

A failed step is not counted as completed. A missing device fails before the
action is attempted and records zero attempts.

## Structured logging

The TCP server writes timestamped log messages to:

```text
logs/hwtest-server.log
```

Example:

```text
2026-09-29T10:59:55Z [INFO] Executing command: relay relay-1 status
```

Supported levels:

- `INFO`
- `WARNING`
- `ERROR`

The logger is thread-safe, allowing concurrent client threads to write complete
log entries.

Generated logs are not committed to Git.

## JSON test reports

Every procedure execution automatically creates a JSON report:

```text
Report written: reports/multi_relay_test-<timestamp>.json
```

Reports contain:

- Procedure name
- Overall `PASS`, `FAIL`, or `CANCELLED` status
- Completed-step count
- Total duration
- Failed-step name
- Overall diagnostic message
- Per-step status
- Attempt count
- Step duration
- Per-step diagnostic message

Example:

```json
{
    "completed_steps": 9,
    "duration_ms": 0,
    "failed_step": null,
    "message": "All steps passed",
    "procedure": "Multi-relay independence test",
    "status": "PASS",
    "steps": [
        {
            "attempts": 1,
            "duration_ms": 0,
            "message": "Step passed",
            "name": "Switch first relay on",
            "status": "PASS"
        }
    ]
}
```

Generated reports are stored in `reports/` and are not committed to Git.

A duration of `0` milliseconds is valid when an operation completes faster than
the clock's millisecond resolution.

## Test

Run all automated tests:

```bash
ctest --test-dir build --output-on-failure
```

The test suite covers:

- Command processing
- Relay state transitions
- Relay registration and lookup
- Configuration data models
- JSON configuration parsing
- Configuration validation
- Device factory behavior
- Configurable initial states
- Transactional registry creation
- Local and server startup failure handling
- Device-aware procedure parsing
- Multi-relay procedure execution
- Retries, timeouts, and cancellation
- Structured execution results
- JSON report generation
- Thread-safe logging
- Network serialization and framing
- TCP transport
- Concurrent clients
- Remote configured-device control
- Remote procedure execution

## Architecture

```text
JSON device configuration
          |
          v
Configuration parser and validation
          |
          v
PlatformConfiguration
          |
          v
Device factory
          |
          v
RelayRegistry
     |             |
     v             v
 relay-1         relay-2
     |             |
     v             v
IRelay          IRelay
     |             |
     v             v
SimulatedRelay  SimulatedRelay
```

Procedure execution:

```text
JSON procedure
      |
      v
Procedure parser
      |
      v
TestProcedure
      |
      v
Test executor
      |
      v
RelayRegistry
      |
      v
Selected device
      |
      v
TestResult
      |
      v
JSON report
```

Distributed command execution:

```text
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
```

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

## License

This project is available under the [MIT License](LICENSE).

Copyright (c) 2026 Atefeh Mohammadpour.
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
