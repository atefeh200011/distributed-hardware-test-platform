# Distributed Hardware Test and Control Platform

A modern C++20 platform for deterministic hardware testing, device control,
communication, and automated test execution.

## Project status

Milestone 1 complete: C++20 project foundation, interactive command-line shell,
and automated command tests.

## Planned capabilities

- Hardware abstraction and simulated device drivers
- JSON-defined test procedures
- Reliable test execution with timeouts, retries, and cancellation
- TCP client/server communication
- Automated tests and structured test reports

## Motivation

I am building this project to develop practical skills in modern C++, hardware communication, and automated testing.

## Requirements

- Ubuntu on WSL2
- GCC 13 or newer
- CMake 3.20 or newer
- Ninja
- Git

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
| `exit` | Exit the application |

## Test

Run all automated tests:

```bash
ctest --test-dir build --output-on-failure
```

## Project structure

```text
.
├── src/
│   ├── command_shell.cpp
│   ├── command_shell.h
│   └── main.cpp
├── tests/
│   └── command_shell_tests.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

