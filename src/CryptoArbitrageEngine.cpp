#include "crypto/CryptoArbitrageEngine.h"
#include "exceptions/Exceptions.h"
#include "common/StringUtils.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

namespace am {

namespace {

static bool is_utc_iso8601_timestamp(const std::string& ts) {
    if (ts.size() != 20) return false;
    if (ts[4]!='-'||ts[7]!='-'||ts[10]!='T'||ts[13]!=':'||ts[16]!=':'||ts[19]!='Z')
        return false;
    for (size_t i : {0,1,2,3,5,6,8,9,11,12,14,15,17,18})
        if (!std::isdigit((unsigned char)ts[i])) return false;
    return true;
}

static char detect_delimiter(const std::string& header) {
    if (header.find('\t') != std::string::npos) return '\t';
    return ',';
}

static std::vector<std::string> split_csv_line(const std::string& line,
                                                char delim) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string tok;
    while (std::getline(ss, tok, delim))
        out.push_back(str::trim_copy(tok));
    return out;
}

static double parse_double(const std::string& raw, const char* field) {
    try { return std::stod(raw); }
    catch (...) {
        throw DataValidationError(
            std::string("Cannot parse '") + field + "': " + raw);
    }
}

} // anonymous namespace

std::vector<CryptoQuote> CryptoArbitrageEngine::load_quotes_csv(
    const std::string& path,
    const std::unordered_map<std::string, double>& fee_overrides,
    double default_fee_bps) const
{
    std::ifstream in(path);
    if (!in) throw CsvError("Cannot open quotes CSV: " + path);

    std::string hdr;
    if (!std::getline(in, hdr)) throw CsvError("Empty CSV: " + path);
    const char delim = detect_delimiter(hdr);
    const auto cols  = split_csv_line(hdr, delim);

    int c_ex = -1, c_sym = -1, c_bid = -1, c_ask = -1, c_fee = -1;
    for (int i = 0; i < (int)cols.size(); ++i) {
        auto h = str::upper_trim_copy(cols[i]);
        if      (h == "EXCHANGE") c_ex  = i;
        else if (h == "SYMBOL")   c_sym = i;
        else if (h == "BID")      c_bid = i;
        else if (h == "ASK")      c_ask = i;
        else if (h == "FEE_BPS")  c_fee = i;
    }
    if (c_ex < 0 || c_sym < 0 || c_bid < 0 || c_ask < 0)
        throw CsvError("Missing required columns in: " + path);

    std::vector<CryptoQuote> quotes;
    std::string line;
    while (std::getline(in, line)) {
        line = str::trim_copy(line);
        if (line.empty()) continue;
        const auto row = split_csv_line(line, delim);
        if ((int)row.size() <= std::max({c_ex, c_sym, c_bid, c_ask})) continue;

        CryptoQuote q;
        q.exchange = str::upper_trim_copy(row[c_ex]);
        q.symbol   = str::upper_trim_copy(row[c_sym]);
        q.bid      = parse_double(row[c_bid], "bid");
        q.ask      = parse_double(row[c_ask], "ask");
        if (q.bid <= 0.0 || q.ask <= 0.0 || q.ask < q.bid)
            throw DataValidationError("Bad bid/ask: " + q.exchange);

        if (c_fee >= 0 && c_fee < (int)row.size() && !row[c_fee].empty())
            q.fee_bps = parse_double(row[c_fee], "fee_bps");
        else {
            auto it = fee_overrides.find(q.exchange);
            q.fee_bps = (it != fee_overrides.end()) ? it->second
                                                     : default_fee_bps;
        }
        quotes.push_back(std::move(q));
    }
    return quotes;
}

std::vector<CryptoOpportunity> CryptoArbitrageEngine::find_opportunities(
    const std::vector<CryptoQuote>& quotes,
    const CryptoMonitorConfig& cfg) const
{
    std::vector<CryptoOpportunity> out;
    for (size_t i = 0; i < quotes.size(); ++i) {
        const auto& buy = quotes[i];
        if (!cfg.symbol_filter.empty() && buy.symbol != cfg.symbol_filter)
            continue;
        for (size_t j = 0; j < quotes.size(); ++j) {
            if (i == j) continue;
            const auto& sell = quotes[j];
            if (sell.symbol   != buy.symbol)  continue;
            if (sell.exchange == buy.exchange) continue;

            const double buy_cost  = buy.ask  * (1.0 + buy.fee_bps  / 10000.0);
            const double sell_gain = sell.bid  * (1.0 - sell.fee_bps / 10000.0);
            const double net       = sell_gain - buy_cost;
            if (net <= cfg.min_net_spread) continue;

            const double net_pct = net / buy_cost;
            if (net_pct <= cfg.min_net_pct) continue;

            CryptoOpportunity o;
            o.symbol        = buy.symbol;
            o.buy_exchange  = buy.exchange;
            o.sell_exchange = sell.exchange;
            o.buy_ask       = buy.ask;
            o.sell_bid      = sell.bid;
            o.gross_spread  = sell.bid - buy.ask;
            o.net_spread    = net;
            o.net_pct       = net_pct;
            out.push_back(std::move(o));
        }
    }
    std::sort(out.begin(), out.end(),
        [](const CryptoOpportunity& a, const CryptoOpportunity& b){
            return a.net_spread > b.net_spread;
        });
    return out;
}

void CryptoArbitrageEngine::write_opportunities_csv(
    const std::string& path,
    const std::string& observed_at,
    const std::vector<CryptoOpportunity>& opps) const
{
    if (!opps.empty() && !is_utc_iso8601_timestamp(observed_at))
        throw DataValidationError(
            "observed_at must be UTC ISO 8601: YYYY-MM-DDTHH:MM:SSZ");
    std::ofstream out(path);
    if (!out) throw CsvError("Cannot write output CSV: " + path);
    out << "observed_at,symbol,buy_exchange,sell_exchange,"
           "buy_ask,sell_bid,gross_spread,net_spread,net_pct\n";
    for (const auto& o : opps)
        out << observed_at     << ","
            << o.symbol        << ","
            << o.buy_exchange  << ","
            << o.sell_exchange << ","
            << o.buy_ask       << ","
            << o.sell_bid      << ","
            << o.gross_spread  << ","
            << o.net_spread    << ","
            << o.net_pct       << "\n";
}

void CryptoArbitrageEngine::reset_opportunities_csv(
    const std::string& path) const
{
    std::ofstream out(path);
    if (!out) throw CsvError("Cannot reset output CSV: " + path);
    out << "observed_at,symbol,buy_exchange,sell_exchange,"
           "buy_ask,sell_bid,gross_spread,net_spread,net_pct\n";
}

void CryptoArbitrageEngine::append_opportunities_csv(
    const std::string& path,
    const std::string& observed_at,
    const std::vector<CryptoOpportunity>& opps) const
{
    std::ofstream out(path, std::ios::app);
    if (!out) throw CsvError("Cannot append to output CSV: " + path);
    for (const auto& o : opps)
        out << observed_at     << ","
            << o.symbol        << ","
            << o.buy_exchange  << ","
            << o.sell_exchange << ","
            << o.buy_ask       << ","
            << o.sell_bid      << ","
            << o.gross_spread  << ","
            << o.net_spread    << ","
            << o.net_pct       << "\n";
}

void CryptoArbitrageEngine::reset_profit_csv(const std::string& path) const {
    std::ofstream out(path);
    if (!out) throw CsvError("Cannot reset profit CSV: " + path);
    out << "observed_at,symbol,buy_exchange,sell_exchange,"
           "net_pct,capital_before,estimated_profit,capital_after\n";
}

double CryptoArbitrageEngine::append_profit_csv(
    const std::string& path,
    const std::string& observed_at,
    double capital_before,
    const std::vector<CryptoOpportunity>& opps) const
{
    if (opps.empty()) return capital_before;
    const auto best_it = std::max_element(opps.begin(), opps.end(),
        [](const CryptoOpportunity& a, const CryptoOpportunity& b){
            return a.net_pct < b.net_pct;
        });
    const auto& best   = *best_it;
    const double profit    = capital_before * best.net_pct;
    const double cap_after = capital_before + profit;

    std::ofstream out(path, std::ios::app);
    if (!out) throw CsvError("Cannot append to profit CSV: " + path);
    out << observed_at        << ","
        << best.symbol        << ","
        << best.buy_exchange  << ","
        << best.sell_exchange << ","
        << best.net_pct       << ","
        << capital_before     << ","
        << profit             << ","
        << cap_after          << "\n";
    return cap_after;
}

} // namespace am
