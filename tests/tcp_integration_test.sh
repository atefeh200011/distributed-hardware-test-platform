#!/usr/bin/env bash

set -euo pipefail

server_executable="$1"
client_executable="$2"

temporary_directory="$(mktemp -d)"
server_log="${temporary_directory}/server.log"
server_pid=""

cleanup()
{
    if [[ -n "${server_pid}" ]] &&
       kill -0 "${server_pid}" 2>/dev/null
    then
        kill "${server_pid}" 2>/dev/null || true
        wait "${server_pid}" 2>/dev/null || true
    fi

    rm -rf "${temporary_directory}"
}

trap cleanup EXIT

"${server_executable}" >"${server_log}" 2>&1 &
server_pid="$!"

server_ready=false

for _ in {1..50}
do
    if grep -q \
        "Hardware test server listening" \
        "${server_log}"
    then
        server_ready=true
        break
    fi

    sleep 0.05
done

if [[ "${server_ready}" != true ]]
then
    echo "FAIL: TCP server did not become ready"
    cat "${server_log}"
    exit 1
fi

relay_on_output="$("${client_executable}" relay on)"

if [[ "${relay_on_output}" != "Relay state: on" ]]
then
    echo "FAIL: relay on response is incorrect"
    echo "Actual: ${relay_on_output}"
    exit 1
fi

relay_status_on_output="$(
    "${client_executable}" relay status
)"

if [[ "${relay_status_on_output}" != "Relay state: on" ]]
then
    echo "FAIL: relay should remain on"
    echo "Actual: ${relay_status_on_output}"
    exit 1
fi

relay_off_output="$("${client_executable}" relay off)"

if [[ "${relay_off_output}" != "Relay state: off" ]]
then
    echo "FAIL: relay off response is incorrect"
    echo "Actual: ${relay_off_output}"
    exit 1
fi

relay_status_off_output="$(
    "${client_executable}" relay status
)"

if [[ "${relay_status_off_output}" != "Relay state: off" ]]
then
    echo "FAIL: relay should remain off"
    echo "Actual: ${relay_status_off_output}"
    exit 1
fi

exit_output="$("${client_executable}" exit)"

if [[ "${exit_output}" != \
      "Shutting down the hardware test platform project." ]]
then
    echo "FAIL: exit response is incorrect"
    echo "Actual: ${exit_output}"
    exit 1
fi

wait "${server_pid}"
server_pid=""

if grep -q \
    "Hardware test server stopped" \
    "${server_log}"
then
    echo "PASS: TCP client/server integration"
else
    echo "FAIL: server did not stop cleanly"
    cat "${server_log}"
    exit 1
fi

