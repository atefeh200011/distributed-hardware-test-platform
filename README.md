# Distributed Hardware Test and Control Platform

A modern C++20 platform for deterministic hardware testing, device control,
communication, and automated test execution.

## Project status

Milestone 3 complete: JSON-defined hardware test procedures, schema validation,
in-memory execution, and CLI-based procedure loading.

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
expectations. Run the included relay smoke test with:

```text
run procedures/relay_smoke_test.json
```

Supported actions:

| Action | Behavior |
| --- | --- |
| `relay_on` | Switch the relay on |
| `relay_off` | Switch the relay off |
| `expect_relay_on` | Fail if the relay is off |
| `expect_relay_off` | Fail if the relay is on |

Example:

```json
{
    "name": "Relay smoke test",
    "steps": [
        {"name": "Switch relay on", "action": "relay_on"},
        {"name": "Verify relay on", "action": "expect_relay_on"},
        {"name": "Switch relay off", "action": "relay_off"},
        {"name": "Verify relay off", "action": "expect_relay_off"}
    ]
}
```

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
