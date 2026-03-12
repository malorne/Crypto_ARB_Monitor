# CLI Contract (crypto-only)

Binary: `arbitrage_monitor`

## Commands

- `run [--config <path>] [--online-crypto true|false] [--no-fixtures true|false] [--start-capital <value>]`
- `watch [--config <path>] [--interval-sec N] [--online-crypto true|false] [--no-fixtures true|false] [--start-capital <value>]`
- `config [--config <path>] [--online-crypto true|false] [--no-fixtures true|false] [--start-capital <value>]`

## Outputs

- `crypto_opportunities.csv`
  - recreated at startup (previous content cleared)
  - appended on each run/watch iteration
- `crypto_profit.csv` (if `profit_calc_enabled=true`)
  - recreated at startup (previous content cleared)
  - appended on each run/watch iteration

## Config keys

- `crypto_quotes_csv`
- `crypto_fees_csv`
- `crypto_output_csv`
- `profit_output_csv`
- `crypto_symbol`
- `crypto_symbols`
- `crypto_exchanges`
- `crypto_default_fee_bps`
- `crypto_min_net_spread`
- `crypto_min_net_pct`
- `profit_calc_enabled`
- `start_capital`
- `crypto_online_source` (`TRADINGVIEW` or direct API fallback)
- `online_crypto_enabled`
- `watch_interval_sec`
- `no_fixtures`

## Path resolution

Relative paths from config are resolved against the config file directory.
