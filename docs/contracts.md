# Crypto Data Contract

## Quotes input (`crypto_quotes_csv`)

Required logical columns:
- `exchange` (aliases: `exchange`, `venue`, `broker`)
- `symbol` (aliases: `symbol`, `ticker`, `pair`)
- `bid` (aliases: `bid`, `bid_price`)
- `ask` (aliases: `ask`, `ask_price`)

Optional:
- `fee_bps` (aliases: `fee_bps`, `commission_bps`)

Rules:
- delimiters: `,`, `;`, tab, `|`
- header matching is case-insensitive
- `bid > 0`, `ask > 0`, `ask >= bid`

## Fees input (`crypto_fees_csv`)

Required logical columns:
- `exchange`
- `fee_bps`

If `fee_bps` is missing in quotes, fee is resolved from:
1. `crypto_fees_csv` by exchange
2. `crypto_default_fee_bps`

## Opportunities output (`crypto_opportunities.csv`)

Columns:
- `observed_at` (UTC timestamp, `YYYY-MM-DDTHH:MM:SSZ`)
- `symbol`
- `buy_exchange`
- `sell_exchange`
- `buy_ask`
- `sell_bid`
- `gross_spread`
- `net_spread`
- `net_pct`
- file is reset once at process start, then opportunities are appended each iteration
- API note: engine report writers require non-empty UTC timestamp for any appended rows

## Profit output (`crypto_profit.csv`)

Columns:
- `observed_at` (UTC timestamp, `YYYY-MM-DDTHH:MM:SSZ`)
- `capital_before`
- `opportunities_count`
- `best_symbol`
- `buy_exchange`
- `sell_exchange`
- `best_net_pct`
- `estimated_profit`
- `capital_after`

Rules:
- produced only when `profit_calc_enabled=true`
- file is reset once at process start, then appended each iteration
- profit uses best opportunity by `net_pct`: `estimated_profit = capital_before * best_net_pct`
- `capital_after` becomes `capital_before` for the next iteration in the same process

Filter policy:
- keep only opportunities with positive `net_spread`
- apply `crypto_min_net_spread`
- apply `crypto_min_net_pct`
- never mix symbols in one opportunity (buy/sell must be same `symbol`)
