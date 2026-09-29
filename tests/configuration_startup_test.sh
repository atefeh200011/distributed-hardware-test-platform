#!/usr/bin/env bash

set -euo pipefail

application_executable="$1"
server_executable="$2"
invalid_configuration_file="$3"

expected_error="Failed to load device configuration: unknown device type: physical_relay"

set +e

application_output="$(
    "${application_executable}" \
        "${invalid_configuration_file}" \
        2>&1
)"
application_status="$?"

set -e

if [[ "${application_status}" -ne 1 ]]
then
    echo "FAIL: application should return exit code 1"
    echo "Actual exit code: ${application_status}"
    echo "Output:"
    echo "${application_output}"
    exit 1
fi

if [[ "${application_output}" != "${expected_error}" ]]
then
    echo "FAIL: application configuration error is incorrect"
    echo "Expected:"
    echo "${expected_error}"
    echo "Actual:"
    echo "${application_output}"
    exit 1
fi

set +e

server_output="$(
    "${server_executable}" \
        "${invalid_configuration_file}" \
        2>&1
)"
server_status="$?"

set -e

if [[ "${server_status}" -ne 1 ]]
then
    echo "FAIL: server should return exit code 1"
    echo "Actual exit code: ${server_status}"
    echo "Output:"
    echo "${server_output}"
    exit 1
fi

if [[ "${server_output}" != "${expected_error}" ]]
then
    echo "FAIL: server configuration error is incorrect"
    echo "Expected:"
    echo "${expected_error}"
    echo "Actual:"
    echo "${server_output}"
    exit 1
fi

echo "PASS: invalid configuration startup handling"
