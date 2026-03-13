// Подключаем crypto/CryptoArbitrageEngine.h.
#include "crypto/CryptoArbitrageEngine.h"

// Подключаем algorithm.
#include <algorithm>
// Подключаем cctype.
#include <cctype>
// Подключаем cmath.
#include <cmath>
// Подключаем cstdlib.
#include <cstdlib>
// Подключаем fstream.
#include <fstream>
// Подключаем iomanip.
#include <iomanip>
// Подключаем string.
#include <string>
// Подключаем unordered_map.
#include <unordered_map>
// Подключаем vector.
#include <vector>

// Подключаем common/StringUtils.h.
#include "common/StringUtils.h"
// Подключаем exceptions/Exceptions.h.
#include "exceptions/Exceptions.h"

// Пространство имен модуля.
namespace am {

// Пространство имен модуля.
namespace {

// задаем константу чтобы значение было стабильным и явным
constexpr double kBpsDenominator = 10000.0;

// is_utc_iso8601_timestamp: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
static bool is_utc_iso8601_timestamp(const std::string& ts) {
    // Пояснение к следующему шагу.
// проверяем условие чтобы отсеять неподходящий сценарий
    if (ts.size() != 20) return false;
// Фиксируем ожидаемое значение.
    const auto is_digit_at = [&](size_t i) {
// Возвращаем вычисленное значение.
        return std::isdigit(static_cast<unsigned char>(ts[i])) != 0;
// Конец объявления типа.
    };
// Возвращаем вычисленное значение.
    return is_digit_at(0) && is_digit_at(1) && is_digit_at(2) && is_digit_at(3) &&
// Значение сохраняется для следующих шагов.
           ts[4] == '-' &&
// is_digit_at: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка данных к следующему шагу.
           is_digit_at(5) && is_digit_at(6) &&
// Значение сохраняется для следующих шагов.
           ts[7] == '-' &&
// is_digit_at: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
           is_digit_at(8) && is_digit_at(9) &&
// Значение сохраняется для следующих шагов.
           ts[10] == 'T' &&
// is_digit_at: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Продолжение общей логики.
           is_digit_at(11) && is_digit_at(12) &&
// Значение сохраняется для следующих шагов.
           ts[13] == ':' &&
// is_digit_at: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка данных к следующему шагу.
           is_digit_at(14) && is_digit_at(15) &&
// Значение сохраняется для следующих шагов.
           ts[16] == ':' &&
// is_digit_at: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
           is_digit_at(17) && is_digit_at(18) &&
// Значение сохраняется для следующих шагов.
           ts[19] == 'Z';
// Конец текущего блока.
}

// detect_delimiter: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
static char detect_delimiter(const std::string& header) {
// Фиксируем ожидаемое значение.
    const std::vector<char> candidates = {',', ';', '\t', '|'};
// Значение сохраняется для следующих шагов.
    size_t best = 0;
// Значение сохраняется для следующих шагов.
    char delimiter = ',';
// Обходим все элементы.
    for (const char c : candidates) {
// count: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const size_t cnt = static_cast<size_t>(std::count(header.begin(), header.end(), c));
// проверяем условие чтобы отсеять неподходящий сценарий
        if (cnt > best) {
// Значение сохраняется для следующих шагов.
            best = cnt;
// Значение сохраняется для следующих шагов.
            delimiter = c;
// Конец текущего блока.
        }
// Конец текущего блока.
    }
// Возвращаем вычисленное значение.
    return delimiter;
// Конец текущего блока.
}

// split_csv_line: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка данных к следующему шагу.
static std::vector<std::string> split_csv_line(const std::string& line, char delimiter) {
// Подготовка к следующему шагу.
    std::vector<std::string> out;
// Подготовка к следующему шагу.
    std::string cell;
// Значение сохраняется для следующих шагов.
    bool in_quotes = false;
// Обходим все элементы.
    for (size_t i = 0; i < line.size(); ++i) {
// Фиксируем ожидаемое значение.
        const char ch = line[i];
// проверяем условие чтобы отсеять неподходящий сценарий
        if (ch == '"') {
// проверяем условие чтобы отсеять неподходящий сценарий
            if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
                cell.push_back('"');
// Подготовка к следующему шагу.
                ++i;
// Поддерживаем линейную логику.
            } else {
// Значение сохраняется для следующих шагов.
                in_quotes = !in_quotes;
// Конец текущего блока.
            }
// пропускаем остаток итерации и берем следующий элемент
            continue;
// Конец текущего блока.
        }
// проверяем условие чтобы отсеять неподходящий сценарий
        if (ch == delimiter && !in_quotes) {
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            out.push_back(str::trim_copy(cell));
// clear: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            cell.clear();
// пропускаем остаток итерации и берем следующий элемент
            continue;
// Конец текущего блока.
        }
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
        cell.push_back(ch);
// Конец текущего блока.
    }
// проверяем условие чтобы отсеять неподходящий сценарий
    if (in_quotes) throw CsvError("Malformed CSV line in crypto quotes: unclosed quote");
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    out.push_back(str::trim_copy(cell));
// Возвращаем вычисленное значение.
    return out;
// Конец текущего блока.
}

// parse_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Продолжение общей логики.
static double parse_double(const std::string& raw, const char* field_name) {
// trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::string s = str::trim_copy(raw);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (s.empty()) throw DataValidationError(std::string("Empty numeric field: ") + field_name);
// Значение сохраняется для следующих шагов.
    char* end = nullptr;
// strtod: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const double v = std::strtod(s.c_str(), &end);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (end == s.c_str() || *end != '\0' || !std::isfinite(v)) {
// бросаем исключение чтобы явно подсветить ошибочный кейс
        throw DataValidationError(std::string("Bad numeric field: ") + field_name);
// Конец текущего блока.
    }
// Возвращаем вычисленное значение.
    return v;
// Конец текущего блока.
}

// header_index: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
static std::unordered_map<std::string, size_t> header_index(const std::vector<std::string>& headers) {
// Подготовка к следующему шагу.
    std::unordered_map<std::string, size_t> idx;
// Обходим все элементы.
    for (size_t i = 0; i < headers.size(); ++i) idx[str::upper_copy(headers[i])] = i;
// Возвращаем вычисленное значение.
    return idx;
// Конец текущего блока.
}

// find_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Продолжение общей логики.
static std::optional<size_t> find_alias(const std::unordered_map<std::string, size_t>& idx,
// Поддерживаем линейную логику.
                                        std::initializer_list<const char*> aliases) {
// Обходим все элементы.
    for (const char* a : aliases) {
// find: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const auto it = idx.find(str::upper_copy(a));
// проверяем условие чтобы отсеять неподходящий сценарий
        if (it != idx.end()) return it->second;
// Конец текущего блока.
    }
// Возвращаем вычисленное значение.
    return std::nullopt;
// Конец текущего блока.
}

// require_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
static size_t require_alias(const std::unordered_map<std::string, size_t>& idx,
// Фиксируем ожидаемое значение.
                            const char* logical_name,
// тут закрываем небольшой кусок общей задачи
                            std::initializer_list<const char*> aliases) {
// find_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto c = find_alias(idx, aliases);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!c.has_value()) throw CsvError(std::string("Missing crypto column: ") + logical_name);
// Возвращаем вычисленное значение.
    return *c;
// Конец текущего блока.
}

// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
static std::string csv_escape(const std::string& in) {
// Значение сохраняется для следующих шагов.
    bool needs_quotes = false;
// Подготовка к следующему шагу.
    std::string out;
// reserve: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    out.reserve(in.size() + 4);
// Обходим все элементы.
    for (const char ch : in) {
// проверяем условие чтобы отсеять неподходящий сценарий
        if (ch == '"' || ch == ',' || ch == '\n' || ch == '\r') needs_quotes = true;
// проверяем условие чтобы отсеять неподходящий сценарий
        if (ch == '"') out.push_back('"');
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
        out.push_back(ch);
// Конец текущего блока.
    }
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!needs_quotes) return out;
// Возвращаем вычисленное значение.
    return "\"" + out + "\"";
// Конец текущего блока.
}

// Рабочий шаг в общей цепочке.
} // закрываем пространство имен
// Путь данных: вход -> проверки -> результат.
// Технический шаг.
std::unordered_map<std::string, double> CryptoArbitrageEngine::load_fees_csv(const std::string& csv_path) const {
// проверяем условие чтобы отсеять неподходящий сценарий
    if (csv_path.empty()) return {};
// in: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::ifstream in(csv_path);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!in) throw CsvError("Cannot open crypto fees file: " + csv_path);

// Подготовка к следующему шагу.
    std::string header;
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!std::getline(in, header)) throw CsvError("Empty crypto fees CSV: " + csv_path);
// detect_delimiter: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const char delimiter = detect_delimiter(header);
// split_csv_line: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    auto headers = split_csv_line(header, delimiter);
// header_index: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto idx = header_index(headers);
// require_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const size_t c_exchange = require_alias(idx, "exchange", {"exchange", "venue", "broker"});
// require_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const size_t c_fee = require_alias(idx, "fee_bps", {"fee_bps", "commission_bps"});

// Подготовка к следующему шагу.
    std::unordered_map<std::string, double> out;
// Подготовка к следующему шагу.
    std::string line;
// Повторяем, пока условие истинно.
    while (std::getline(in, line)) {
// проверяем условие чтобы отсеять неподходящий сценарий
        if (!line.empty() && line.back() == '\r') line.pop_back();
// проверяем условие чтобы отсеять неподходящий сценарий
        if (str::trim_copy(line).empty()) continue;
// split_csv_line: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        auto cols = split_csv_line(line, delimiter);
// проверяем условие чтобы отсеять неподходящий сценарий
        if (cols.size() != headers.size()) throw CsvError("Crypto fees row has wrong column count");
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const std::string ex = str::upper_trim_copy(cols[c_exchange]);
// parse_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const double fee = parse_double(cols[c_fee], "fee_bps");
// Значение сохраняется для следующих шагов.
        out[ex] = fee;
// Конец текущего блока.
    }
// Возвращаем вычисленное значение.
    return out;
// Конец текущего блока.
}

// load_quotes_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Продолжение общей логики.
std::vector<CryptoQuote> CryptoArbitrageEngine::load_quotes_csv(
// Фиксируем ожидаемое значение.
    const std::string& csv_path,
// Фиксируем ожидаемое значение.
    const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Фиксируем ожидаемое значение.
    const double default_fee_bps) const {

// in: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::ifstream in(csv_path);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!in) throw CsvError("Cannot open crypto quotes file: " + csv_path);

// Подготовка к следующему шагу.
    std::string header;
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!std::getline(in, header)) throw CsvError("Empty crypto quotes CSV: " + csv_path);
// detect_delimiter: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const char delimiter = detect_delimiter(header);
// split_csv_line: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    auto headers = split_csv_line(header, delimiter);
// header_index: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto idx = header_index(headers);

// require_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const size_t c_exchange = require_alias(idx, "exchange", {"exchange", "venue", "broker"});
// require_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const size_t c_symbol = require_alias(idx, "symbol", {"symbol", "ticker", "pair"});
// require_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const size_t c_bid = require_alias(idx, "bid", {"bid", "bid_price"});
// require_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const size_t c_ask = require_alias(idx, "ask", {"ask", "ask_price"});
// find_alias: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto c_fee = find_alias(idx, {"fee_bps", "commission_bps"});

// Подготовка к следующему шагу.
    std::vector<CryptoQuote> out;
// Подготовка к следующему шагу.
    std::string line;
// Повторяем, пока условие истинно.
    while (std::getline(in, line)) {
// проверяем условие чтобы отсеять неподходящий сценарий
        if (!line.empty() && line.back() == '\r') line.pop_back();
// проверяем условие чтобы отсеять неподходящий сценарий
        if (str::trim_copy(line).empty()) continue;
// split_csv_line: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        auto cols = split_csv_line(line, delimiter);
// проверяем условие чтобы отсеять неподходящий сценарий
        if (cols.size() != headers.size()) throw CsvError("Crypto quote row has wrong column count");

// Подготовка к следующему шагу.
        CryptoQuote q;
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        q.exchange = str::upper_trim_copy(cols[c_exchange]);
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        q.symbol = str::upper_trim_copy(cols[c_symbol]);
// parse_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        q.bid = parse_double(cols[c_bid], "bid");
// parse_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        q.ask = parse_double(cols[c_ask], "ask");
// проверяем условие чтобы отсеять неподходящий сценарий
        if (q.bid <= 0.0 || q.ask <= 0.0 || q.ask < q.bid) {
// бросаем исключение чтобы явно подсветить ошибочный кейс
            throw DataValidationError("Bad crypto bid/ask");
// Конец текущего блока.
        }
// проверяем условие чтобы отсеять неподходящий сценарий
        if (c_fee.has_value()) {
// parse_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
            q.fee_bps = parse_double(cols[*c_fee], "fee_bps");
// Технический шаг.
        } else {
// find: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
            const auto it = per_exchange_fee_bps.find(q.exchange);
// end: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
            q.fee_bps = (it != per_exchange_fee_bps.end()) ? it->second : default_fee_bps;
// Конец текущего блока.
        }
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
        out.push_back(std::move(q));
// Конец текущего блока.
    }
// Возвращаем вычисленное значение.
    return out;
// Конец текущего блока.
}

// find_opportunities: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Продолжение общей логики.
std::vector<CryptoOpportunity> CryptoArbitrageEngine::find_opportunities(
// Фиксируем ожидаемое значение.
    const std::vector<CryptoQuote>& quotes,
// Фиксируем ожидаемое значение.
    const CryptoMonitorConfig& cfg) const {

// Подготовка к следующему шагу.
    std::vector<CryptoQuote> filtered;
// reserve: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    filtered.reserve(quotes.size());
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::string symbol = str::upper_trim_copy(cfg.symbol_filter);
// Обходим все элементы.
    for (const auto& q : quotes) {
// проверяем условие чтобы отсеять неподходящий сценарий
        if (!symbol.empty() && str::upper_copy(q.symbol) != symbol) continue;
// проверяем условие чтобы отсеять неподходящий сценарий
        if (!cfg.allowed_exchanges.empty() && cfg.allowed_exchanges.count(str::upper_copy(q.exchange)) == 0) continue;
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
        filtered.push_back(q);
// Конец текущего блока.
    }

// Подготовка к следующему шагу.
    std::vector<CryptoOpportunity> out;
// Обходим все элементы.
    for (size_t i = 0; i < filtered.size(); ++i) {
// Обходим все элементы.
        for (size_t j = 0; j < filtered.size(); ++j) {
// проверяем условие чтобы отсеять неподходящий сценарий
            if (i == j) continue;
// Фиксируем ожидаемое значение.
            const auto& buy = filtered[i];
// Фиксируем ожидаемое значение.
            const auto& sell = filtered[j];
// проверяем условие чтобы отсеять неподходящий сценарий
            if (str::upper_copy(buy.exchange) == str::upper_copy(sell.exchange)) continue;
// проверяем условие чтобы отсеять неподходящий сценарий
            if (str::upper_copy(buy.symbol) != str::upper_copy(sell.symbol)) continue;

// Фиксируем ожидаемое значение.
            const double buy_cost = buy.ask * (1.0 + buy.fee_bps / kBpsDenominator);
// Фиксируем ожидаемое значение.
            const double sell_gain = sell.bid * (1.0 - sell.fee_bps / kBpsDenominator);
// Фиксируем ожидаемое значение.
            const double gross = sell.bid - buy.ask;
// Фиксируем ожидаемое значение.
            const double net = sell_gain - buy_cost;
// Фиксируем ожидаемое значение.
            const double net_pct = (buy_cost > 0.0) ? (net / buy_cost) : 0.0;

// проверяем условие чтобы отсеять неподходящий сценарий
            if (net <= 0.0) continue;
// проверяем условие чтобы отсеять неподходящий сценарий
            if (net < cfg.min_net_spread) continue;
// проверяем условие чтобы отсеять неподходящий сценарий
            if (net_pct < cfg.min_net_pct) continue;

// Подготовка к следующему шагу.
            CryptoOpportunity o;
// Значение сохраняется для следующих шагов.
            o.symbol = buy.symbol;
// Значение сохраняется для следующих шагов.
            o.buy_exchange = buy.exchange;
// Значение сохраняется для следующих шагов.
            o.sell_exchange = sell.exchange;
// Значение сохраняется для следующих шагов.
            o.buy_ask = buy.ask;
// Значение сохраняется для следующих шагов.
            o.sell_bid = sell.bid;
// Значение сохраняется для следующих шагов.
            o.gross_spread = gross;
// Значение сохраняется для следующих шагов.
            o.net_spread = net;
// Значение сохраняется для следующих шагов.
            o.net_pct = net_pct;
// push_back: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            out.push_back(std::move(o));
// Конец текущего блока.
        }
// Конец текущего блока.
    }

// sort: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка данных к следующему шагу.
    std::sort(out.begin(), out.end(), [](const CryptoOpportunity& a, const CryptoOpportunity& b) {
// Возвращаем вычисленное значение.
        return a.net_spread > b.net_spread;
// Подготовка к следующему шагу.
    });
// Возвращаем вычисленное значение.
    return out;
// Конец текущего блока.
}

// reset_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Технический шаг.
void CryptoArbitrageEngine::reset_opportunities_csv(const std::string& path) const {
// out: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::ofstream out(path, std::ios::trunc);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!out) throw CsvError("Cannot write crypto opportunities report: " + path);
// доклеиваем следующую часть данных в выходной поток
    out << "observed_at,symbol,buy_exchange,sell_exchange,buy_ask,sell_bid,gross_spread,net_spread,net_pct\n";
// Конец текущего блока.
}

// append_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
void CryptoArbitrageEngine::append_opportunities_csv(
// Фиксируем ожидаемое значение.
    const std::string& path,
// Фиксируем ожидаемое значение.
    const std::string& observed_at,
// Фиксируем ожидаемое значение.
    const std::vector<CryptoOpportunity>& opps) const {

// out: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::ofstream out(path, std::ios::app);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!out) throw CsvError("Cannot append crypto opportunities report: " + path);
    // Пояснение к текущему шагу.
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!opps.empty() && !is_utc_iso8601_timestamp(observed_at)) {
// бросаем исключение чтобы явно подсветить ошибочный кейс
        throw DataValidationError("observed_at must be UTC ISO 8601: YYYY-MM-DDTHH:MM:SSZ");
// Конец текущего блока.
    }
// setprecision: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
    out << std::fixed << std::setprecision(8);
// Обходим все элементы.
    for (const auto& o : opps) {
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
        out << csv_escape(observed_at) << ","
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
            << csv_escape(o.symbol) << ","
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
            << csv_escape(o.buy_exchange) << ","
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
            << csv_escape(o.sell_exchange) << ","
// доклеиваем следующую часть данных в выходной поток
            << o.buy_ask << ","
// доклеиваем следующую часть данных в выходной поток
            << o.sell_bid << ","
// доклеиваем следующую часть данных в выходной поток
            << o.gross_spread << ","
// доклеиваем следующую часть данных в выходной поток
            << o.net_spread << ","
// доклеиваем следующую часть данных в выходной поток
            << o.net_pct << "\n";
// Конец текущего блока.
    }
// Конец текущего блока.
}

// reset_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Технический шаг.
void CryptoArbitrageEngine::reset_profit_csv(const std::string& path) const {
// out: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::ofstream out(path, std::ios::trunc);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!out) throw CsvError("Cannot write crypto profit report: " + path);
// доклеиваем следующую часть данных в выходной поток
    out << "observed_at,capital_before,opportunities_count,best_symbol,buy_exchange,sell_exchange,best_net_pct,estimated_profit,capital_after\n";
// Конец текущего блока.
}

// append_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Шаг текущего сценария.
double CryptoArbitrageEngine::append_profit_csv(
// Фиксируем ожидаемое значение.
    const std::string& path,
// Фиксируем ожидаемое значение.
    const std::string& observed_at,
// Фиксируем ожидаемое значение.
    const double capital_before,
// Фиксируем ожидаемое значение.
    const std::vector<CryptoOpportunity>& opps) const {

// out: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::ofstream out(path, std::ios::app);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!out) throw CsvError("Cannot append crypto profit report: " + path);
// setprecision: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
    out << std::fixed << std::setprecision(8);

// проверяем условие чтобы отсеять неподходящий сценарий
    if (opps.empty()) {
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
        out << csv_escape(observed_at) << ","
// доклеиваем следующую часть данных в выходной поток
            << capital_before << ","
// доклеиваем следующую часть данных в выходной поток
            << 0 << ","
// доклеиваем следующую часть данных в выходной поток
            << ",,,"
// доклеиваем следующую часть данных в выходной поток
            << 0.0 << ","
// доклеиваем следующую часть данных в выходной поток
            << 0.0 << ","
// доклеиваем следующую часть данных в выходной поток
            << capital_before << "\n";
// Возвращаем вычисленное значение.
        return capital_before;
// Конец текущего блока.
    }

// max_element: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto best_it = std::max_element(opps.begin(), opps.end(), [](const CryptoOpportunity& a, const CryptoOpportunity& b) {
// Возвращаем вычисленное значение.
        return a.net_pct < b.net_pct;
// Подготовка к следующему шагу.
    });
// Фиксируем ожидаемое значение.
    const CryptoOpportunity& best = *best_it;
// Фиксируем ожидаемое значение.
    const double estimated_profit = capital_before * best.net_pct;
// Фиксируем ожидаемое значение.
    const double capital_after = capital_before + estimated_profit;

// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
    out << csv_escape(observed_at) << ","
// доклеиваем следующую часть данных в выходной поток
        << capital_before << ","
// size: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
        << opps.size() << ","
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
        << csv_escape(best.symbol) << ","
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
        << csv_escape(best.buy_exchange) << ","
// csv_escape: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
        << csv_escape(best.sell_exchange) << ","
// доклеиваем следующую часть данных в выходной поток
        << best.net_pct << ","
// доклеиваем следующую часть данных в выходной поток
        << estimated_profit << ","
// доклеиваем следующую часть данных в выходной поток
        << capital_after << "\n";
// Возвращаем вычисленное значение.
    return capital_after;
// Конец текущего блока.
}

// write_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Поддерживаем линейную логику.
void CryptoArbitrageEngine::write_opportunities_csv(
// Фиксируем ожидаемое значение.
    const std::string& path,
// Фиксируем ожидаемое значение.
    const std::string& observed_at,
// Фиксируем ожидаемое значение.
    const std::vector<CryptoOpportunity>& opps) const {
// reset_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    reset_opportunities_csv(path);
// append_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    append_opportunities_csv(path, observed_at, opps);
// Конец текущего блока.
}

// Продолжение общей логики.
} // закрываем пространство имен am


