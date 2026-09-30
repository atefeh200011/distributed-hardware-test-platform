# Changelog

All notable changes to the Distributed Hardware Test and Control Platform are
documented in this file.

The project follows Semantic Versioning.

## [1.0.0] - 2026-09-30

### Added

- Hardware abstraction through the `IRelay` interface.
- Deterministic simulated relay implementation.
- Named relay registry with independent device state.
- Interactive command-line interface for local device control.
- JSON-defined hardware test procedures.
- Named-device selection in procedure steps.
- Procedure schema validation and descriptive parsing errors.
- Step retry configuration.
- Step timeout detection.
- Cooperative test cancellation.
- Structured procedure and step execution results.
- JSON test report serialization and report-file generation.
- Thread-safe structured server logging.
- JSON device configuration.
- Configurable simulated-device creation and initial state.
- TCP client and concurrent TCP server.
- Newline-delimited JSON network framing.
- JSON request and response protocol.
- Persistent server-side hardware state across client connections.
- Remote command and procedure execution.
- Unit, integration, startup-validation, and concurrent-client tests.
- Optional AddressSanitizer and UndefinedBehaviorSanitizer builds.
- GitHub Actions CI for regular and sanitized builds.
- MIT License.

### Changed

- Centralized compiler warnings, C++20 settings, include paths, and sanitizer
  configuration in CMake.
- Normalized command whitespace before command processing.
- Updated the application version to `1.0.0`.

[1.0.0]: https://github.com/atefeh200011/distributed-hardware-test-platform/releases/tag/v1.0.0
