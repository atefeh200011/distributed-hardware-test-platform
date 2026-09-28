#!/usr/bin/env bash

set -euo pipefail

server_executable="$1"
client_executable="$2"
procedure_file="$3"
multi_procedure_file="$4"

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

relay_list_output="$(
    "${client_executable}" relays
)"

expected_relay_list=$(
    printf '%s\n' \
        "Available relays:" \
        "  relay-1" \
        "  relay-2"
)

if [[ "${relay_list_output}" != "${expected_relay_list}" ]]
then
    echo "FAIL: remote relay list is incorrect"
    echo "Expected:"
    echo "${expected_relay_list}"
    echo "Actual:"
    echo "${relay_list_output}"
    exit 1
fi

relay_1_on_output="$(
    "${client_executable}" relay relay-1 on
)"

if [[ "${relay_1_on_output}" != \
      "Relay relay-1 state: on" ]]
then
    echo "FAIL: relay-1 on response is incorrect"
    echo "Actual: ${relay_1_on_output}"
    exit 1
fi

relay_1_status_output="$(
    "${client_executable}" relay relay-1 status
)"

if [[ "${relay_1_status_output}" != \
      "Relay relay-1 state: on" ]]
then
    echo "FAIL: relay-1 should remain on"
    echo "Actual: ${relay_1_status_output}"
    exit 1
fi

relay_2_status_output="$(
    "${client_executable}" relay relay-2 status
)"

if [[ "${relay_2_status_output}" != \
      "Relay relay-2 state: off" ]]
then
    echo "FAIL: relay-2 should initially be off"
    echo "Actual: ${relay_2_status_output}"
    exit 1
fi

concurrent_client_pids=()

for client_index in {1..8}
do
    "${client_executable}" \
        relay relay-1 status \
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

    if [[ "${concurrent_output}" != \
          "Relay relay-1 state: on" ]]
    then
        echo \
            "FAIL: concurrent client ${client_index} received incorrect state"
        echo "Actual: ${concurrent_output}"
        exit 1
    fi
done

relay_2_on_output="$(
    "${client_executable}" relay relay-2 on
)"

if [[ "${relay_2_on_output}" != \
      "Relay relay-2 state: on" ]]
then
    echo "FAIL: relay-2 on response is incorrect"
    echo "Actual: ${relay_2_on_output}"
    exit 1
fi

relay_1_after_relay_2_output="$(
    "${client_executable}" relay relay-1 status
)"

if [[ "${relay_1_after_relay_2_output}" != \
      "Relay relay-1 state: on" ]]
then
    echo "FAIL: changing relay-2 affected relay-1"
    echo "Actual: ${relay_1_after_relay_2_output}"
    exit 1
fi

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

if [[ "${procedure_result_output}" != \
      "${expected_procedure_output}" ]]
then
    echo "FAIL: remote JSON procedure output is incorrect"
    echo "Expected:"
    echo "${expected_procedure_output}"
    echo "Actual:"
    echo "${procedure_result_output}"
    exit 1
fi

if [[ ! "${report_line}" =~ ^Report\ written:\ reports/.+\.json$ ]]
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

if ! grep -q '"status": "PASS"' "${report_path}"
then
    echo "FAIL: generated report does not contain PASS status"
    cat "${report_path}"
    exit 1
fi

rm -f "${report_path}"

relay_1_final_state="$(
    "${client_executable}" relay relay-1 status
)"

if [[ "${relay_1_final_state}" != \
      "Relay relay-1 state: off" ]]
then
    echo "FAIL: procedure should leave relay-1 off"
    echo "Actual: ${relay_1_final_state}"
    exit 1
fi

relay_2_final_state="$(
    "${client_executable}" relay relay-2 status
)"

if [[ "${relay_2_final_state}" != \
      "Relay relay-2 state: on" ]]
then
    echo "FAIL: procedure should not change relay-2"
    echo "Actual: ${relay_2_final_state}"
    exit 1
fi

relay_2_off_output="$(
    "${client_executable}" relay relay-2 off
)"

if [[ "${relay_2_off_output}" != \
      "Relay relay-2 state: off" ]]
then
    echo "FAIL: relay-2 off response is incorrect"
    echo "Actual: ${relay_2_off_output}"
    exit 1
fi

multi_procedure_output="$(
    "${client_executable}" run "${multi_procedure_file}"
)"

expected_multi_procedure_output=$(
    printf '%s\n' \
        "Running procedure: Multi-relay independence test" \
        "Step: Switch first relay on" \
        "Step: Switch second relay on" \
        "Step: Verify first relay on" \
        "Step: Verify second relay on" \
        "Step: Switch first relay off" \
        "Step: Verify first relay off" \
        "Step: Verify second relay remains on" \
        "Step: Switch second relay off" \
        "Step: Verify second relay off" \
        "Result: PASS"
)

multi_procedure_result_output="$(
    printf '%s\n' "${multi_procedure_output}" |
        head -n 11
)"

multi_report_line="$(
    printf '%s\n' "${multi_procedure_output}" |
        tail -n 1
)"

if [[ "${multi_procedure_result_output}" != \
      "${expected_multi_procedure_output}" ]]
then
    echo "FAIL: remote multi-relay procedure output is incorrect"
    echo "Expected:"
    echo "${expected_multi_procedure_output}"
    echo "Actual:"
    echo "${multi_procedure_result_output}"
    exit 1
fi

if [[ ! "${multi_report_line}" =~ ^Report\ written:\ reports/.+\.json$ ]]
then
    echo "FAIL: multi-relay report path is incorrect"
    echo "Actual: ${multi_report_line}"
    exit 1
fi
multi_report_path="${multi_report_line#Report written: }"

if [[ ! -f "${multi_report_path}" ]]
then
    echo "FAIL: multi-relay report file does not exist"
    echo "Expected file: ${multi_report_path}"
    exit 1
fi

if ! grep -q '"status": "PASS"' "${multi_report_path}"
then
    echo "FAIL: multi-relay report does not contain PASS status"
    cat "${multi_report_path}"
    exit 1
fi

if ! grep -q \
    '"procedure": "Multi-relay independence test"' \
    "${multi_report_path}"
then
    echo "FAIL: multi-relay report has incorrect procedure name"
    cat "${multi_report_path}"
    exit 1
fi

rm -f "${multi_report_path}"

relay_1_after_multi="$(
    "${client_executable}" relay relay-1 status
)"

if [[ "${relay_1_after_multi}" != \
      "Relay relay-1 state: off" ]]
then
    echo "FAIL: multi-relay procedure should leave relay-1 off"
    echo "Actual: ${relay_1_after_multi}"
    exit 1
fi

relay_2_after_multi="$(
    "${client_executable}" relay relay-2 status
)"

if [[ "${relay_2_after_multi}" != \
      "Relay relay-2 state: off" ]]
then
    echo "FAIL: multi-relay procedure should leave relay-2 off"
    echo "Actual: ${relay_2_after_multi}"
    exit 1
fi

multi_report_path="${multi_report_line#Report written: }"



missing_relay_output="$(
    "${client_executable}" relay missing status
)"

if [[ "${missing_relay_output}" != \
      "Relay not found: missing" ]]
then
    echo "FAIL: missing relay response is incorrect"
    echo "Actual: ${missing_relay_output}"
    exit 1
fi

exit_output="$(
    "${client_executable}" exit
)"

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
