# Distributed Hardware Test and Control Platform

A modern C++20 platform for deterministic hardware testing, device control,
communication, and automated test execution.

## Project status

Milestone 5 complete: distributed device control through a concurrent TCP
client/server architecture, JSON request/response messaging, persistent simulated
hardware state, and automated network integration testing.

## Planned capabilities

- Structured logging and machine-readable test reports
- Additional simulated and physical device drivers
- Sanitizers, static analysis, and GitHub Actions
- Docker-based server deployment
- Optional Python client
- Optional ESP32, STM32, or Raspberry Pi integration

## Motivation

I am building this project to develop practical skills in modern C++, hardware
communication, and automated testing.

## Requirements

- Ubuntu on WSL2
- GCC 13 or newer
- CMake 3.20 or newer
- Ninja
- Git
- nlohmann/json 3.11 or newer

## Build

Configure the project with CMake and Ninja:

```bash
cmake -S . -B build -G Ninja
```

Compile the application and tests:

```bash
cmake --build build
```

## Run

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

## Distributed TCP operation

The project includes a TCP server and command-line client. The server listens
only on the local loopback interface at `127.0.0.1:5050`.

Start the server in one terminal:

```bash
./build/hwtest_server
```

Send commands from another terminal:

```bash
./build/hwtest_client relay on
./build/hwtest_client relay status
./build/hwtest_client relay off
```

Run a JSON-defined test procedure remotely:

```bash
./build/hwtest_client run procedures/relay_smoke_test.json
```

Stop the server cleanly:

```bash
./build/hwtest_client exit
```

The server preserves simulated device state across separate client connections.
Multiple clients can connect concurrently. A mutex protects shared hardware state
and prevents concurrent command execution from causing C++ data races.

## Network protocol

The client and server exchange newline-delimited JSON messages over TCP. A
newline identifies the end of each message because TCP transports an unstructured
stream of bytes rather than preserving application message boundaries.

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

The protocol validates required fields and JSON types before executing commands.
Responses repeat the request identifier so clients can associate responses with
their requests.

The shared transport layer handles partial sends, interrupted system calls,
peer disconnection, newline framing, and a 64 KiB maximum message size.

## Test

Run all automated tests:

```bash
ctest --test-dir build --output-on-failure
```

## JSON test procedures

Test procedures define an ordered sequence of deterministic hardware actions and
expectations. Each procedure contains a name and a list of steps that the
execution engine processes in order.

Run the included relay smoke test from the command-line shell:

```text
run procedures/relay_smoke_test.json
```

A procedure has the following structure:

```json
{
    "name": "Relay smoke test",
    "steps": [
        {
            "name": "Switch relay on",
            "action": "relay_on",
            "timeout_ms": 500
        },
        {
            "name": "Verify relay on",
            "action": "expect_relay_on",
            "retries": 2,
            "timeout_ms": 500
        },
        {
            "name": "Switch relay off",
            "action": "relay_off",
            "timeout_ms": 500
        },
        {
            "name": "Verify relay off",
            "action": "expect_relay_off",
            "retries": 2,
            "timeout_ms": 500
        }
    ]
}
```

Each step contains:

| Field | Required | Description |
| --- | --- | --- |
| `name` | Yes | Human-readable name shown in test output |
| `action` | Yes | Deterministic hardware action or expectation |
| `retries` | No | Number of additional attempts after failure |
| `timeout_ms` | No | Maximum duration of each attempt in milliseconds |

Supported actions:

| Action | Behavior |
| --- | --- |
| `relay_on` | Switch the relay on |
| `relay_off` | Switch the relay off |
| `expect_relay_on` | Fail if the relay is off |
| `expect_relay_off` | Fail if the relay is on |

Reliability fields use these defaults:

| Field | Default | Validation |
| --- | --- | --- |
| `retries` | `0` | Must be a non-negative integer |
| `timeout_ms` | `1000` | Must be a positive integer |

For example, the following step has one initial attempt and up to two additional
retry attempts. Every attempt has a 500-millisecond deadline:

```json
{
    "name": "Verify relay on",
    "action": "expect_relay_on",
    "retries": 2,
    "timeout_ms": 500
}
```

The JSON loader validates procedure names, step names, actions, retry counts, and
timeouts before execution. Invalid files are rejected with a descriptive error
instead of being partially executed.

## Execution reliability

The execution engine returns a structured result containing:

- Overall pass or failure status
- Number of successfully completed steps
- Name of the failed step, when applicable
- Human-readable result details
- Cancellation status

When a step fails and still has available retries, the executor performs another
attempt. For example, a retry value of `2` permits three total attempts:

```text
Attempt 1: initial attempt
Attempt 2: first retry
Attempt 3: second retry
```

Every attempt is measured using C++'s monotonic clock. If an attempt takes longer
than its configured `timeout_ms`, the step fails with a timeout result.

Cancellation is cooperative and thread-safe. The executor checks an atomic
cancellation signal:

- Before starting a step
- Before starting each retry attempt
- After a synchronous device operation returns

The current timeout mechanism detects an exceeded deadline after a synchronous
device operation returns. It does not forcibly terminate an active hardware
operation. This avoids leaving a background operation accessing hardware through
an object that may no longer exist.

## Architecture

The command shell depends on the `IRelay` interface rather than a specific
hardware implementation. The current `SimulatedRelay` driver provides
deterministic in-memory behavior for development and automated testing.

Future physical relay drivers can implement the same interface without changing
the command-processing logic.

The TCP server converts network requests into the same command-processing calls
used by the local CLI. This keeps networking separate from device-control and
test-execution logic.

Each accepted client is handled by a separate C++ thread. Network reception and
response transmission can therefore proceed concurrently, while a mutex
serializes access to the shared relay and test engine.

The current server is a local development implementation. It uses a fixed
loopback address and port, processes one request per connection, and does not yet
provide authentication or encryption.

## Project structure

```text
.
├── procedures/
│   └── relay_smoke_test.json
├── src/
│   ├── command_shell.cpp
│   ├── command_shell.h
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
│   └── test_result.h
├── tests/
│   ├── command_shell_tests.cpp
│   ├── network_framing_tests.cpp
│   ├── network_protocol_tests.cpp
│   ├── simulated_relay_tests.cpp
│   ├── tcp_integration_test.sh
│   ├── tcp_transport_tests.cpp
│   ├── test_executor_tests.cpp
│   ├── test_procedure_json_tests.cpp
│   └── test_procedure_tests.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```
