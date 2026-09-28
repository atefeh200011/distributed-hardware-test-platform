# Distributed Hardware Test and Control Platform

A modern C++20 platform for deterministic hardware testing, named-device
control, network communication, automated test execution, structured logging,
and machine-readable test reports.

## Project status

Milestone 7 complete: named relay registration, independent multi-device
control, device-aware JSON procedures, and local and remote multi-relay test
execution.

## Current capabilities

- Hardware abstraction through the `IRelay` interface
- Deterministic simulated relay drivers
- Named relay registration and lookup
- Independent state for multiple relay devices
- Interactive command-line device control
- JSON-defined, device-aware test procedures
- Test execution with retries and timeout detection
- Cooperative test cancellation
- Structured procedure and step results
- TCP client/server communication
- Concurrent TCP client handling
- Persistent simulated device state across connections
- JSON network request and response messages
- Thread-safe structured server logging
- Machine-readable JSON test reports
- Automated unit and integration tests

## Planned capabilities

- Additional simulated hardware device types
- Physical hardware driver implementations
- Dynamic device configuration
- Test execution history and report analysis
- Authentication and encrypted network communication
- Continuous integration and automated releases
- Graphical monitoring and control interface

## Motivation

I am building this project to develop practical skills in modern C++, hardware
abstraction, communication, networking, concurrency, automated testing, and
software architecture.

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

## Local command-line application

Start the interactive command-line shell:

```bash
./build/hwtest
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
Hardware Test Platform version 0.1.0
hwtest> relays
Available relays:
  relay-1
  relay-2
hwtest> relay relay-1 on
Relay relay-1 state: on
hwtest> relay relay-2 status
Relay relay-2 state: off
hwtest> relay relay-1 off
Relay relay-1 state: off
hwtest> exit
Shutting down the hardware test platform project.
```

Each registered relay maintains its own independent state.

## Device registry

The `RelayRegistry` owns relay implementations and makes them available through
unique names such as:

```text
relay-1
relay-2
```

The registry provides:

- Relay registration
- Lookup by name
- Duplicate-name rejection
- Missing-device detection
- Deterministic sorted device listings
- Shared access through the `IRelay` abstraction

The command shell and test executor depend on the registry instead of depending
directly on a single simulated relay.

Future relay drivers can implement `IRelay` and be registered without changing
the command-processing or procedure-execution logic.

## TCP client and server

The TCP server listens on `127.0.0.1:5050`.

Start the server in one terminal:

```bash
./build/hwtest_server
```

Use another terminal to list devices:

```bash
./build/hwtest_client relays
```

Control named relays remotely:

```bash
./build/hwtest_client relay relay-1 on
./build/hwtest_client relay relay-1 status
./build/hwtest_client relay relay-2 on
./build/hwtest_client relay relay-2 status
./build/hwtest_client relay relay-1 off
./build/hwtest_client relay relay-2 off
```

Execute a JSON procedure remotely:

```bash
./build/hwtest_client run procedures/relay_smoke_test.json
```

Execute a multi-relay procedure remotely:

```bash
./build/hwtest_client run procedures/multi_relay_test.json
```

Stop the server cleanly:

```bash
./build/hwtest_client exit
```

The server preserves the state of every registered relay between client
connections and handles multiple client connections concurrently.

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

The request ID allows the client to verify that the response belongs to its
request.

## JSON test procedures

Test procedures define ordered sequences of named-device actions and
expectations.

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

When `device` is omitted, it defaults to `relay-1`.

When `retries` is omitted, it defaults to `0`.

When `timeout_ms` is omitted, it defaults to `1000` milliseconds.

### Single-relay procedure

Run the included relay smoke test:

```text
run procedures/relay_smoke_test.json
```

Example step:

```json
{
    "name": "Verify relay on",
    "device": "relay-1",
    "action": "expect_relay_on",
    "retries": 2,
    "timeout_ms": 500
}
```

### Multi-relay procedure

Run the multi-relay independence test:

```text
run procedures/multi_relay_test.json
```

The procedure controls both registered relays:

```json
{
    "name": "Multi-relay independence test",
    "steps": [
        {
            "name": "Switch first relay on",
            "device": "relay-1",
            "action": "relay_on",
            "timeout_ms": 500
        },
        {
            "name": "Switch second relay on",
            "device": "relay-2",
            "action": "relay_on",
            "timeout_ms": 500
        },
        {
            "name": "Verify first relay on",
            "device": "relay-1",
            "action": "expect_relay_on",
            "timeout_ms": 500
        },
        {
            "name": "Verify second relay on",
            "device": "relay-2",
            "action": "expect_relay_on",
            "timeout_ms": 500
        }
    ]
}
```

Before each step, the executor looks up the configured device in the registry.
If the device does not exist, execution fails with a structured error such as:

```text
Relay not found: missing
```

## Reliable test execution

The test executor processes procedure steps in order and produces a structured
result.

Execution features include:

- Named-device resolution
- Missing-device detection
- Retry handling
- Step timeout detection
- Cooperative cancellation
- Completed-step tracking
- Failed-step identification
- Overall procedure duration
- Per-step duration and attempt counts
- Diagnostic result messages

A failed step is not counted as completed. If all configured attempts fail, the
procedure stops and returns a failed result.

A missing device fails before the action is attempted and records an attempt
count of zero.

Cancellation is cooperative: the executor checks for cancellation between
operations and stops safely when cancellation is requested.

## Structured logging

The TCP server writes timestamped log messages to:

```text
logs/hwtest-server.log
```

Each entry contains a UTC timestamp, severity level, and message:

```text
2026-09-24T10:59:55Z [INFO] Executing command: relay relay-1 status
```

Supported log levels include:

- `INFO`
- `WARNING`
- `ERROR`

The logger is thread-safe, allowing concurrent TCP client threads to write
complete entries without corrupting the log file.

The `logs/` directory contains generated runtime data and is not committed to
Git.

## JSON test reports

Running a JSON test procedure automatically creates a machine-readable report:

```text
hwtest> run procedures/multi_relay_test.json
Running procedure: Multi-relay independence test
Step: Switch first relay on
Step: Switch second relay on
Step: Verify first relay on
Step: Verify second relay on
Step: Switch first relay off
Step: Verify first relay off
Step: Verify second relay remains on
Step: Switch second relay off
Step: Verify second relay off
Result: PASS
Report written: reports/multi_relay_test-<timestamp>.json
```

Report generation also occurs when a procedure runs remotely through the TCP
client.

Every report contains:

- Procedure name
- Overall `PASS`, `FAIL`, or `CANCELLED` status
- Number of completed steps
- Total procedure duration
- Failed-step name when applicable
- Overall diagnostic message
- Per-step status
- Per-step attempt count
- Per-step duration
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

Report filenames contain timestamps so multiple executions do not overwrite
one another.

Generated reports are stored in `reports/`, which is not committed to Git.

A duration of `0` milliseconds is valid when a simulated operation completes
faster than the clock's millisecond resolution.

## Test

Run all automated tests:

```bash
ctest --test-dir build --output-on-failure
```

The test suite covers:

- Command processing
- Named relay registration and lookup
- Duplicate and invalid relay registration
- Independent simulated relay state
- Device-aware JSON procedure parsing
- Default device selection
- Invalid and missing device handling
- Multi-relay procedure execution
- Retry and timeout behavior
- Cooperative cancellation
- Detailed execution results
- JSON report serialization and file writing
- Structured logger behavior
- Concurrent logger writes
- Network protocol serialization
- Newline-delimited message framing
- TCP transport
- Concurrent TCP clients
- Remote named-relay control
- Remote single-relay procedure execution
- Remote multi-relay procedure execution
- Automatic report generation

## Architecture

The `IRelay` interface separates hardware operations from their
implementations.

`SimulatedRelay` provides deterministic in-memory behavior for development and
testing.

`RelayRegistry` owns and identifies multiple relay implementations:

```text
RelayRegistry
    |
    +---- relay-1 ----> IRelay ----> SimulatedRelay
    |
    +---- relay-2 ----> IRelay ----> SimulatedRelay
```

The device-aware procedure path is:

```text
JSON procedure
      |
      v
Procedure parser
      |
      v
TestProcedure with named devices
      |
      v
Test executor
      |
      v
RelayRegistry
      |
      v
Selected IRelay implementation
      |
      v
TestResult
      |
      v
JSON report
```

The distributed command path is:

```text
TCP client
    |
    v
JSON request
    |
    v
Newline-delimited TCP transport
    |
    v
Concurrent TCP server
    |
    v
Command shell
    |
    v
RelayRegistry
    |
    +----> Named relay control
    |
    +----> Multi-device test execution
    |
    +----> JSON report generation
```

A server-side command mutex protects shared device state while concurrent
clients are handled by separate threads.

## Project structure

```text
.
├── procedures/
│   ├── multi_relay_test.json
│   └── relay_smoke_test.json
├── src/
│   ├── command_shell.cpp
│   ├── command_shell.h
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
│   └── test_result.h
├── tests/
│   ├── command_shell_tests.cpp
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
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Milestones

- Milestone 1: Project foundation and interactive command shell
- Milestone 2: Hardware abstraction and simulated relay
- Milestone 3: JSON-defined test procedures
- Milestone 4: Reliable execution, retries, timeouts, and cancellation
- Milestone 5: Concurrent TCP client/server communication
- Milestone 6: Structured logging and machine-readable test reports
- Milestone 7: Named multi-device registration, control, and execution