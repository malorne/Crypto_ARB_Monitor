cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED MONITOR_BIN)
    message(FATAL_ERROR "MONITOR_BIN not set")
endif()

set(TMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/args_tmp")
file(MAKE_DIRECTORY "${TMP_DIR}")

set(CONFIG_FILE "${TMP_DIR}/app.conf")
file(WRITE "${CONFIG_FILE}"
"crypto_quotes_csv = /nonexistent/quotes.csv\n"
"online_crypto_enabled = false\n"
)

execute_process(
    COMMAND "${MONITOR_BIN}" config --config "${CONFIG_FILE}"
    RESULT_VARIABLE ret
    OUTPUT_VARIABLE out_stdout
)
if(NOT ret EQUAL 0)
    message(FATAL_ERROR "config command failed")
endif()
string(FIND "${out_stdout}" "online_crypto_enabled" has_key)
if(has_key EQUAL -1)
    message(FATAL_ERROR "config output missing expected key")
endif()

execute_process(
    COMMAND "${MONITOR_BIN}" run --config "${CONFIG_FILE}" --unknown-opt 1
    RESULT_VARIABLE bad_ret
    ERROR_VARIABLE  bad_err
)
if(bad_ret EQUAL 0)
    message(FATAL_ERROR "Expected non-zero exit for unknown option")
endif()
string(FIND "${bad_err}" "Unknown CLI option: --unknown-opt" bad_opt_pos)
if(bad_opt_pos EQUAL -1)
    message(FATAL_ERROR "Expected error for unknown option, got: ${bad_err}")
endif()

message(STATUS "CLI config/args test PASSED")
