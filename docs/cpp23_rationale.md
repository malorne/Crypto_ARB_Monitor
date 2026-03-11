# C++23 Feature Rationale

This document explains why each required language/library feature is used in the project.

## `std::unique_ptr`
- Used for short-lived, exclusive ownership of online connector instances in the runtime path.
- Why: ownership is single and explicit, no accidental copies.
- Location: `src/main.cpp` (`std::unique_ptr<am::IMarketDataConnectors>`).

## `std::shared_ptr`
- Used for shared lifetime of the arbitrage engine across runtime lambdas.
- Why: avoids dangling references when lambdas capture engine and makes lifetime explicit.
- Location: `src/main.cpp` (`std::shared_ptr<am::IArbitrageEngine>`).

## `std::optional`
- Used for config values, parser outputs, and nullable fields from online feeds.
- Why: distinguishes "missing value" from default primitive values without sentinel hacks.
- Locations: `include/config/ConfigManager.h`, `include/online/MarketDataConnectors.h`, parsing code in `src/MarketDataConnectors.cpp`.

## `std::variant` + `std::visit`
- Used to model mutually exclusive quote-source modes (`offline`, `tradingview`, `direct-api`).
- Why: type-safe dispatch without manual enum-switch + casts, and exhaustive compile-time handling.
- Location: `src/main.cpp` (`QuotesSource`, `detect_quotes_source`, `std::visit` in `execute_once`).

## RAII
- Used for file streams, process pipes, and resource cleanup through object lifetime.
- Why: deterministic cleanup even on exceptions.
- Locations: `std::ifstream/std::ofstream` throughout core; `PipeReader` in `src/MarketDataConnectors.cpp`.

## `constexpr`
- Used for immutable compile-time constants.
- Why: intent clarity and compile-time guarantees.
- Locations: `src/CryptoArbitrageEngine.cpp` (`kBpsDenominator`), `src/main.cpp` (`kTradingViewSource`).

## Exceptions
- Used for configuration, CSV/data validation, and transport errors.
- Why: simplifies error propagation from deep parsing/networking layers to CLI boundary.
- Locations: `include/exceptions/Exceptions.h`, throw sites in `src/*.cpp`, centralized handling in `main`.
