# Distributed Hardware Test and Control Platform

A modern C++20 platform for deterministic hardware testing, device control,
network communication, automated test execution, structured logging, and
machine-readable test reports.

## Project status

Milestone 6 complete: thread-safe structured logging, detailed per-step
execution results, persistent server logs, and automatically generated JSON
test reports.

## Current capabilities

- Hardware abstraction through the `IRelay` interface
- Deterministic simulated relay driver
- Interactive command-line control
- JSON-defined test procedures
- Test execution with retries and timeout detection
- Cooperative test cancellation
- Structured procedure and step results
- TCP client/server communication
- Concurrent TCP client handling
- Persistent simulated device state across client connections
- JSON network request and response messages
- Thread-safe structured server logging
- Machine-readable JSON test reports
- Automated unit and integration tests

## Planned capabilities

- Multiple simulated and physical hardware devices
- Test execution history and report analysis
- Authentication and encrypted network communication
- Continuous integration and automated releases
- Graphical monitoring and control interface

## Motivation

I am building this project to develop practical skills in modern C++, hardware
communication, networking, concurrency, automated testing, and software
architecture.

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

Compile the application, server, client, and tests:

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
| `relay on` | Switch the simulated relay on |
| `relay off` | Switch the simulated relay off |
| `relay status` | Show the simulated relay state |
| `run <file>` | Load and execute a JSON test procedure |
| `exit` | Exit the application |

Example:

```text
Hardware Test Platform version 0.1.0
hwtest> relay on
Relay state: on
hwtest> relay status
Relay state: on
hwtest> relay off
Relay state: off
hwtest> exit
Shutting down the hardware test platform project.
```

## TCP client and server

The TCP server listens on `127.0.0.1:5050`.

Start the server in one terminal:

```bash
./build/hwtest_server
```

Send commands from another terminal:

```bash
./build/hwtest_client status
./build/hwtest_client relay on
./build/hwtest_client relay status
./build/hwtest_client relay off
```

Execute a JSON procedure remotely:

```bash
./build/hwtest_client run procedures/relay_smoke_test.json
```

Stop the server cleanly:

```bash
./build/hwtest_client exit
```

The server preserves the simulated relay state between client connections and
handles multiple client connections concurrently.

## Network protocol

The client and server exchange newline-delimited JSON messages over TCP.

Example request:

```json
{
    "request_id": "client-request-001",
    "command": "relay status"
}
```

Example response:

```json
{
    "request_id": "client-request-001",
    "success": true,
    "output": "Relay state: off\n"
}
```

The response request ID allows a client to verify that the response belongs to
its request.

## JSON test procedures

Test procedures define an ordered sequence of hardware actions and
expectations. Each procedure contains a name and a list of steps.

Run the included relay smoke test from the local shell:

```text
run procedures/relay_smoke_test.json
```

Or run it through the TCP client:

```bash
./build/hwtest_client run procedures/relay_smoke_test.json
```

Supported actions:

| Action | Behavior |
| --- | --- |
| `relay_on` | Switch the relay on |
| `relay_off` | Switch the relay off |
| `expect_relay_on` | Fail if the relay is off |
| `expect_relay_off` | Fail if the relay is on |

Each step may also contain:

| Property | Description |
| --- | --- |
| `retries` | Number of additional attempts after the first failure |
| `timeout_ms` | Maximum permitted step duration in milliseconds |

Example procedure:

```json
{
    "name": "Relay smoke test",
    "steps": [
        {
            "name": "Switch relay on",
            "action": "relay_on"
        },
        {
            "name": "Verify relay on",
            "action": "expect_relay_on",
            "retries": 2,
            "timeout_ms": 500
        },
        {
            "name": "Switch relay off",
            "action": "relay_off"
        },
        {
            "name": "Verify relay off",
            "action": "expect_relay_off"
        }
    ]
}
```

When `retries` is omitted, it defaults to `0`. When `timeout_ms` is omitted, it
defaults to `1000` milliseconds.

## Reliable test execution

The test executor processes procedure steps in order and produces a structured
result.

Execution features include:

- Retry handling for failed steps
- Step timeout detection
- Cooperative cancellation
- Completed-step tracking
- Failed-step identification
- Overall procedure duration
- Per-step duration and attempt counts
- Diagnostic result messages

A failed step is not counted as completed. If all configured attempts fail, the
procedure stops and returns a failed result.

Cancellation is cooperative: the executor checks the cancellation request
between operations and stops safely when cancellation is requested.

## Structured logging

The TCP server writes timestamped log messages to:

```text
logs/hwtest-server.log
```

Each entry contains a UTC timestamp, severity level, and message:

```text
2026-09-24T10:59:55Z [INFO] Executing command: relay status
```

Supported log levels include:

- `INFO`
- `WARNING`
- `ERROR`

The logger is thread-safe, allowing concurrent TCP client threads to write
complete log entries without corrupting the log file.

The `logs/` directory contains generated runtime data and is not committed to
Git.

## JSON test reports

Running a JSON test procedure automatically creates a machine-readable report:

```text
hwtest> run procedures/relay_smoke_test.json
Running procedure: Relay smoke test
Step: Switch relay on
Step: Verify relay on
Step: Switch relay off
Step: Verify relay off
Result: PASS
Report written: reports/relay_smoke_test-<timestamp>.json
```

The same report generation occurs when a procedure is executed remotely through
the TCP client.

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

Example report:

```json
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
```

Report filenames contain timestamps so multiple executions do not overwrite
one another.

Generated reports are stored in `reports/`. This directory is not committed to
Git.

A duration of `0` milliseconds is valid when a simulated operation completes
faster than the clock's millisecond resolution.

## Test

Run all automated tests:

```bash
ctest --test-dir build --output-on-failure
```

The test suite covers:

- Command processing
- Simulated relay state transitions
- Test procedure models
- JSON procedure parsing and validation
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
- Remote JSON procedure execution
- Automatic remote report generation

## Architecture

The command shell depends on the `IRelay` interface rather than a specific
hardware implementation. The current `SimulatedRelay` provides deterministic
in-memory behavior for development and automated testing.

Future physical relay drivers can implement the same interface without changing
the command-processing or test-execution logic.

The test executor is separated from JSON parsing and report generation:

```text
JSON procedure
      |
      v
Procedure parser
      |
      v
TestProcedure model
      |
      v
Test executor -----> IRelay -----> SimulatedRelay
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
    +-----> Simulated relay
    |
    +-----> Test executor
    |
    +-----> JSON report
```

## Project structure

```text
.
├── procedures/
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
│   ├── simulated_relay_tests.cpp
│   ├── tcp_integration_test.sh
│   ├── tcp_transport_tests.cpp
│   ├── test_executor_tests.cpp
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