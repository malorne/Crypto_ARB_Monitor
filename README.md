# Crypto ARB Monitor — My Fork / Showcase

> **Forked from a team project built Feb 26 – Mar 12, 2025.**  
> This repo is my personal copy of a collaborative C++23 CLI tool for cross-exchange crypto arbitrage monitoring. I highlight my specific contributions below.

---

## What the project does

**Crypto ARB Monitor** tracks price differences for the same cryptocurrency across multiple exchanges (Binance, Kraken, Bitstamp, TradingView) and calculates whether an arbitrage opportunity is actually profitable **after fees**. It is a monitor and calculator — not a trading bot.

Key outputs:
- `crypto_opportunities.csv` — all detected cross-exchange pairs with gross/net spreads
- `crypto_profit.csv` — best opportunity per iteration with compounded capital growth

---

## My Contributions (Konstantin Ryadinsky)

**25 commits** | CLI layer · Error handling · Documentation · CI/CD

### 1. CLI Interface (`main.cpp`)

Designed and implemented the full command-line interface:

| Command | Description |
|---------|-------------|
| `run` | Single pass — fetches quotes, computes arbitrage, writes CSV, exits with code 0 |
| `watch` | Continuous loop with configurable interval; fault-tolerant — a failed iteration logs the error and continues without crashing the loop |
| `config` | Prints the resolved effective configuration for quick debugging |

The `watch` mode fault-tolerance was a deliberate design decision: real-world API calls fail intermittently, and a monitor that crashes on a single bad response is useless.

**Example usage:**
```bash
# Single run
./build/arbitrage_monitor run --config config/app.conf

# Continuous monitoring every 30 seconds
./build/arbitrage_monitor watch --config config/app.conf --interval-sec 30

# Inspect resolved config
./build/arbitrage_monitor config --config config/app.conf
```

---

### 2. Exception Hierarchy (`include/Exceptions.h`)

Defined a typed exception hierarchy so CLI boundary code can present users with actionable, specific error messages instead of generic crashes:

```
std::exception
└── ConfigError         — bad or missing app.conf
└── CsvError            — malformed or unreadable CSV input
└── DataValidationError — quotes that fail sanity checks
```

Each module throws its specific type; `main.cpp` catches at the top level and maps each to a human-readable message.

---

### 3. GitHub Actions CI (`.github/workflows/`)

Set up the CI pipeline that runs on every push and pull request:

1. CMake configure with `-DAM_BUILD_TESTS=ON`
2. Ninja build
3. `ctest --output-on-failure`

If any test fails the pipeline goes red and merging is blocked. During development CI caught several regressions where a change in one module silently broke another.

---

### 4. End-to-End Tests (`tests/`)

Wrote two CMake-level CLI e2e tests that exercise the full binary (not just units):

**`run_cli_e2e`**
- Runs the binary with an offline fixture CSV (6 BTC quotes across Binance, Kraken, Bitstamp, FxPro, OANDA, Pepperstone)
- Verifies that `crypto_opportunities.csv` has the correct header
- Checks that timestamps are in UTC format
- Deterministic — used as the regression baseline

**`run_cli_config_and_args`**
- Verifies that path resolution works correctly end-to-end
- Asserts that an unknown CLI flag causes exit with a non-zero error code (proper UX for bad input)

These complement the 21 GoogleTest unit tests written by teammates and cover the integration layer that unit tests miss.

---

### 5. Architecture: Output Layer

Defined the output slice of the system's layered architecture:

```
CLI & Config (my layer)
    ↓
Data Layer (connectors, quote normalization)
    ↓
Arbitrage Engine (pair comparison, net_spread calculation)
    ↓
Output Layer (my layer)
    ├── CSV append with UTC observed_at timestamp
    └── Console display with iteration header
```

In `watch` mode the loop pauses and repeats; in `run` mode it exits after one pass. Each module has a single responsibility — the output layer doesn't touch business logic.

---

### 6. Documentation

Wrote and maintained:
- The original project `README.md`
- Module-level interface contracts (header comments describing expected inputs/outputs)
- `docs/cpp23_rationale.md` — rationale for C++23 feature choices (`std::variant`, `std::optional`, `constexpr`, RAII, smart pointers)

---

## Tech Stack

| Area | Technology |
|------|------------|
| Language | C++23 |
| Build | CMake 3.20 + Ninja |
| Testing | GoogleTest (unit) + CTest (e2e) |
| CI | GitHub Actions |
| Data sources | TradingView scanner API, Binance REST, Kraken REST, Bitstamp REST, offline CSV |

---

## Project Stats

- **Team size:** 3 developers
- **Timeline:** 2 weeks (Feb 26 – Mar 12, 2025)
- **Total commits:** 67 across 3 authors
- **My commits:** 25
- **Tests:** 21 unit tests + 2 e2e CLI tests
- **Platforms:** macOS, Linux

---

## Build & Run

```bash
# Configure and build (with tests)
cmake -S . -B build -DAM_BUILD_TESTS=ON
cmake --build build

# Run all tests
ctest --test-dir build --output-on-failure

# Run the monitor (offline mode)
./build/arbitrage_monitor run --config config/app.conf

# Run the monitor (online, TradingView)
./build/arbitrage_monitor run --config config/app.conf --online-crypto true

# Continuous watch, every 30s
./build/arbitrage_monitor watch --config config/app.conf --interval-sec 30
```

---

## Original Repository

This is a fork of the team project: [Flidison/Crypto_ARB_Monitor](https://github.com/Flidison/Crypto_ARB_Monitor)
