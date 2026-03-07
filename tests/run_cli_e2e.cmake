cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED MONITOR_BIN)
    message(FATAL_ERROR "MONITOR_BIN not set")
endif()

set(TMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/e2e_tmp")
file(MAKE_DIRECTORY "${TMP_DIR}")

# buy BINANCE ask=1000.00 fee=8bps  -> buy_cost=1000.80
# sell KRAKEN bid=1010.00 fee=8bps  -> sell_gain=1009.19
# net = +8.39 > 0  -> opportunity exists
set(QUOTES_FILE "${TMP_DIR}/quotes.csv")
file(WRITE "${QUOTES_FILE}"
"exchange,symbol,bid,ask,fee_bps\n"
"BINANCE,BTCUSD,999.00,1000.00,8.0\n"
"KRAKEN,BTCUSD,1010.00,1011.00,8.0\n"
"BITSTAMP,BTCUSD,1005.00,1006.00,10.0\n"
)

set(OUT_FILE    "${TMP_DIR}/opportunities.csv")
set(CONFIG_FILE "${TMP_DIR}/app.conf")
file(WRITE "${CONFIG_FILE}"
"crypto_quotes_csv = quotes.csv\n"
"crypto_output_csv = opportunities.csv\n"
"online_crypto_enabled = false\n"
)

execute_process(
    COMMAND "${MONITOR_BIN}" run --config "${CONFIG_FILE}"
    RESULT_VARIABLE ret
    OUTPUT_VARIABLE out_stdout
    ERROR_VARIABLE  out_stderr
)
if(NOT ret EQUAL 0)
    message(FATAL_ERROR "CLI run failed: ${out_stderr}")
endif()

file(READ "${OUT_FILE}" out_csv)
string(FIND "${out_csv}" "observed_at,symbol,buy_exchange" has_header)
if(has_header EQUAL -1)
    message(FATAL_ERROR "Missing CSV header in output:\n${out_csv}")
endif()

# Portable regex без {n} квантификаторов (совместимость с CMake < 3.9)
string(REGEX MATCH
    "[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z,BTCUSD,"
    has_timestamp_and_symbol
    "${out_csv}"
)
if(NOT has_timestamp_and_symbol)
    message(FATAL_ERROR "No valid timestamp+symbol row in output:\n${out_csv}")
endif()

message(STATUS "CLI e2e test PASSED")
