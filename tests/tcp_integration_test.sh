#!/usr/bin/env bash

set -euo pipefail

server_executable="$1"
client_executable="$2"
procedure_file="$3"

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

concurrent_client_pids=()

for client_index in {1..8}
do
    "${client_executable}" relay status \
        >"${temporary_directory}/client-${client_index}.log" &
    concurrent_client_pids+=("$!")
done

for client_pid in "${concurrent_client_pids[@]}"
do
    wait "${client_pid}"
done

for client_index in {1..8}
do
    concurrent_output="$(
        cat "${temporary_directory}/client-${client_index}.log"
    )"

    if [[ "${concurrent_output}" != "Relay state: on" ]]
    then
        echo \
            "FAIL: concurrent client ${client_index} received incorrect state"
        echo "Actual: ${concurrent_output}"
        exit 1
    fi
done

procedure_output="$(
    "${client_executable}" run "${procedure_file}"
)"

expected_procedure_output=$(
    printf '%s\n' \
        "Running procedure: Relay smoke test" \
        "Step: Switch relay on" \
        "Step: Verify relay on" \
        "Step: Switch relay off" \
        "Step: Verify relay off" \
        "Result: PASS"
)

procedure_result_output="$(
    printf '%s\n' "${procedure_output}" |
        head -n 6
)"

report_line="$(
    printf '%s\n' "${procedure_output}" |
        tail -n 1
)"

if [[ "${procedure_result_output}" != "${expected_procedure_output}" ]]
then
    echo "FAIL: remote JSON procedure output is incorrect"
    echo "Expected:"
    echo "${expected_procedure_output}"
    echo "Actual:"
    echo "${procedure_result_output}"
    exit 1
fi

if [[ ! "${report_line}" =~ \
^Report\ written:\ reports/.+\.json$ ]]
then
    echo "FAIL: remote procedure report path is incorrect"
    echo "Actual: ${report_line}"
    exit 1
fi

report_path="${report_line#Report written: }"

if [[ ! -f "${report_path}" ]]
then
    echo "FAIL: remote procedure report file does not exist"
    echo "Expected file: ${report_path}"
    exit 1
fi

if grep -q '"status": "PASS"' "${report_path}"
then
    :
else
    echo "FAIL: generated remote report does not contain PASS status"
    cat "${report_path}"
    exit 1
fi

rm -f "${report_path}"

procedure_final_state="$(
    "${client_executable}" relay status
)"

if [[ "${procedure_final_state}" != "Relay state: off" ]]
then
    echo "FAIL: remote procedure should leave relay off"
    echo "Actual: ${procedure_final_state}"
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

