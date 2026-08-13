# Distributed Hardware Test and Control Platform

A modern C++20 platform for deterministic hardware testing, device control,
communication, and automated test execution.

## Project status

Milestone 4 complete: reliable test execution with structured results, retry
handling, step timeout detection, and cooperative cancellation.

## Planned capabilities

- Hardware abstraction and simulated device drivers
- JSON-defined test procedures
- Reliable test execution with timeouts, retries, and cancellation
- TCP client/server communication
- Automated tests and structured test reports

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

## Project structure

```text
.
├── procedures/
│   └── relay_smoke_test.json
├── src/
│   ├── command_shell.cpp
│   ├── test_result.h
│   ├── command_shell.h
│   ├── main.cpp
│   ├── relay.h
│   ├── simulated_relay.cpp
│   ├── simulated_relay.h
│   ├── test_executor.cpp
│   ├── test_executor.h
│   ├── test_procedure.h
│   ├── test_procedure_json.cpp
│   └── test_procedure_json.h
├── tests/
│   ├── command_shell_tests.cpp
│   ├── simulated_relay_tests.cpp
│   ├── test_executor_tests.cpp
│   ├── test_procedure_json_tests.cpp
│   └── test_procedure_tests.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```
