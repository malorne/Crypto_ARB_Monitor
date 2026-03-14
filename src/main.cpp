// Подключаем algorithm.
#include <algorithm>
// Подключаем chrono.
#include <chrono>
// Подключаем cctype.
#include <cctype>
// Подключаем ctime.
#include <ctime>
// Подключаем filesystem.
#include <filesystem>
// Подключаем iomanip.
#include <iomanip>
// Подключаем iostream.
#include <iostream>
// Подключаем memory.
#include <memory>
// Подключаем optional.
#include <optional>
// Подключаем sstream.
#include <sstream>
// Подключаем string.
#include <string>
// Подключаем string_view.
#include <string_view>
// Подключаем thread.
#include <thread>
// Подключаем type_traits.
#include <type_traits>
// Подключаем unordered_set.
#include <unordered_set>
// Подключаем variant.
#include <variant>
// Подключаем vector.
#include <vector>

// Подключаем common/StringUtils.h.
#include "common/StringUtils.h"
// Подключаем config/ConfigManager.h.
#include "config/ConfigManager.h"
// Подключаем crypto/CryptoArbitrageEngine.h.
#include "crypto/CryptoArbitrageEngine.h"
// Подключаем exceptions/Exceptions.h.
#include "exceptions/Exceptions.h"
// Подключаем online/MarketDataConnectors.h.
#include "online/MarketDataConnectors.h"

// Пространство имен модуля.
namespace {

// EffectiveConfig: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// этой строкой фиксируем промежуточный результат
struct EffectiveConfig {
// Значение сохраняется для следующих шагов.
    std::string config_path = "config/app.conf";
// Подготовка к следующему шагу.
    std::string crypto_quotes_csv;
// Подготовка к следующему шагу.
    std::string crypto_fees_csv;
// Значение сохраняется для следующих шагов.
    std::string crypto_output_csv = "crypto_opportunities.csv";
// Значение сохраняется для следующих шагов.
    std::string crypto_symbol = "BTCUSD";
// Значение сохраняется для следующих шагов.
    std::string crypto_symbols_csv = "BTCUSD";
// Значение сохраняется для следующих шагов.
    std::string crypto_exchanges_csv = "BINANCE,KRAKEN,BITSTAMP,FXPRO,OANDA,PEPPERSTONE";
// Значение сохраняется для следующих шагов.
    std::string crypto_online_source = "TRADINGVIEW";
// Значение сохраняется для следующих шагов.
    std::string profit_output_csv = "crypto_profit.csv";
// Значение сохраняется для следующих шагов.
    double crypto_default_fee_bps = 10.0;
// Значение сохраняется для следующих шагов.
    double crypto_min_net_spread = 0.0;
// Значение сохраняется для следующих шагов.
    double crypto_min_net_pct = 0.0;
// Значение сохраняется для следующих шагов.
    double start_capital = 1000.0;
// Значение сохраняется для следующих шагов.
    int watch_interval_sec = 5;
// Значение сохраняется для следующих шагов.
    bool profit_calc_enabled = false;
// Значение сохраняется для следующих шагов.
    bool online_crypto_enabled = false;
// Значение сохраняется для следующих шагов.
    bool no_fixtures = false;
// Конец объявления типа.
};

// задаем константу чтобы значение было стабильным и явным
constexpr std::string_view kTradingViewSource = "TRADINGVIEW";

// Пояснение к следующему шагу.
// OfflineQuotesSource: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Подготовка к следующему шагу.
struct OfflineQuotesSource {};
// TradingViewQuotesSource: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Подготовка к следующему шагу.
struct TradingViewQuotesSource {};
// DirectExchangeQuotesSource: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Подготовка к следующему шагу.
struct DirectExchangeQuotesSource {};
// даём удобное короткое имя для следующего кода
using QuotesSource = std::variant<OfflineQuotesSource, TradingViewQuotesSource, DirectExchangeQuotesSource>;

// detect_quotes_source: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Шаг текущего сценария.
QuotesSource detect_quotes_source(const EffectiveConfig& cfg) {
// проверяем условие чтобы отсеять неподходящий сценарий
    if (!cfg.online_crypto_enabled) return OfflineQuotesSource{};
// проверяем условие чтобы отсеять неподходящий сценарий
    if (am::str::upper_trim_copy(cfg.crypto_online_source) == kTradingViewSource) return TradingViewQuotesSource{};
// Возвращаем вычисленное значение.
    return DirectExchangeQuotesSource{};
// Конец текущего блока.
}

// parse_bool_cli: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Рабочий шаг в общей цепочке.
bool parse_bool_cli(const std::string& s) {
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::string normalized = am::str::upper_trim_copy(s);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (normalized == "1" || normalized == "TRUE") return true;
// проверяем условие чтобы отсеять неподходящий сценарий
    if (normalized == "0" || normalized == "FALSE") return false;
// бросаем исключение чтобы явно подсветить ошибочный кейс
    throw am::ConfigError("Invalid bool override value: " + s);
// Конец текущего блока.
}

// resolve_from_config: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
std::string resolve_from_config(const std::string& config_path, const std::string& p) {
// проверяем условие чтобы отсеять неподходящий сценарий
    if (p.empty()) return p;
// pp: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::filesystem::path pp(p);
// проверяем условие чтобы отсеять неподходящий сценарий
    if (pp.is_absolute()) return pp.string();
// absolute: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::filesystem::path cfg = std::filesystem::absolute(std::filesystem::path(config_path));
// parent_path: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::filesystem::path base = cfg.parent_path();
// Возвращаем вычисленное значение.
    return (base / pp).lexically_normal().string();
// Конец текущего блока.
}

// is_fixture_or_test_path: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка данных к следующему шагу.
bool is_fixture_or_test_path(const std::string& p) {
// проверяем условие чтобы отсеять неподходящий сценарий
    if (p.empty()) return false;
// pp: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::filesystem::path pp(p);
// filename: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    std::string file = pp.filename().string();
// lexically_normal: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    std::string full = pp.lexically_normal().generic_string();
// Обходим все элементы.
    for (char& ch : file) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
// Обходим все элементы.
    for (char& ch : full) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
// проверяем условие чтобы отсеять неподходящий сценарий
    if (file.rfind("fixture_", 0) == 0) return true;
// проверяем условие чтобы отсеять неподходящий сценарий
    if (file.rfind("test_", 0) == 0) return true;
// проверяем условие чтобы отсеять неподходящий сценарий
    if (file.contains("_fixture")) return true;
// Возвращаем вычисленное значение.
    return full.contains("/tests/");
// Конец текущего блока.
}

// parse_csv_set: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка данных к следующему шагу.
std::unordered_set<std::string> parse_csv_set(const std::string& csv) {
// Подготовка к следующему шагу.
    std::unordered_set<std::string> out;
// ss: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::stringstream ss(csv);
// Подготовка к следующему шагу.
    std::string token;
// Повторяем, пока условие истинно.
    while (std::getline(ss, token, ',')) {
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const std::string value = am::str::upper_trim_copy(token);
// проверяем условие чтобы отсеять неподходящий сценарий
        if (!value.empty()) out.insert(value);
// Конец текущего блока.
    }
// Возвращаем вычисленное значение.
    return out;
// Конец текущего блока.
}

// parse_csv_list: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Шаг текущего сценария.
std::vector<std::string> parse_csv_list(const std::string& csv) {
// Подготовка к следующему шагу.
    std::vector<std::string> out;
// Подготовка к следующему шагу.
    std::unordered_set<std::string> seen;
// ss: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::stringstream ss(csv);
// Подготовка к следующему шагу.
    std::string token;
// Повторяем, пока условие истинно.
    while (std::getline(ss, token, ',')) {
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const std::string value = am::str::upper_trim_copy(token);
// проверяем условие чтобы отсеять неподходящий сценарий
        if (value.empty()) continue;
// проверяем условие чтобы отсеять неподходящий сценарий
        if (seen.insert(value).second) out.push_back(value);
// Конец текущего блока.
    }
// Возвращаем вычисленное значение.
    return out;
// Конец текущего блока.
}

// utc_now_iso8601: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка данных к следующему шагу.
std::string utc_now_iso8601() {
// now: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto now = std::chrono::system_clock::now();
// to_time_t: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const std::time_t tt = std::chrono::system_clock::to_time_t(now);
// Подготовка к следующему шагу.
    std::tm tm{};
// тут выбираем платформенный путь компиляции
#if defined(_WIN32)
// gmtime_s: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    gmtime_s(&tm, &tt);
// здесь переключаемся на альтернативную платформенную ветку
#else
// gmtime_r: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    gmtime_r(&tt, &tm);
// закрываем условный блок препроцессора
#endif
// Подготовка к следующему шагу.
    std::ostringstream os;
// put_time: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
    os << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
// Возвращаем вычисленное значение.
    return os.str();
// Конец текущего блока.
}

// print_usage: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
void print_usage() {
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout
// доклеиваем следующую часть данных в выходной поток
        << "Usage:\n"
// доклеиваем следующую часть данных в выходной поток
        << "  arbitrage_monitor run [--config config/app.conf] [--online-crypto true|false] [--no-fixtures true|false] [--start-capital 1000]\n"
// доклеиваем следующую часть данных в выходной поток
        << "  arbitrage_monitor watch [--config config/app.conf] [--interval-sec 2] [--online-crypto true|false] [--no-fixtures true|false] [--start-capital 1000]\n"
// доклеиваем следующую часть данных в выходной поток
        << "  arbitrage_monitor config [--config config/app.conf] [--online-crypto true|false] [--no-fixtures true|false] [--start-capital 1000]\n";
// Конец текущего блока.
}

// print_effective_config: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Продолжение общей логики.
void print_effective_config(const EffectiveConfig& c) {
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "command=config\n";
// absolute: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "config_path=" << std::filesystem::absolute(c.config_path).string() << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_quotes_csv=" << c.crypto_quotes_csv << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_fees_csv=" << c.crypto_fees_csv << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_output_csv=" << c.crypto_output_csv << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "profit_output_csv=" << c.profit_output_csv << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_symbol=" << c.crypto_symbol << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_symbols=" << c.crypto_symbols_csv << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_exchanges=" << c.crypto_exchanges_csv << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_default_fee_bps=" << c.crypto_default_fee_bps << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_min_net_spread=" << c.crypto_min_net_spread << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_min_net_pct=" << c.crypto_min_net_pct << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "start_capital=" << c.start_capital << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "crypto_online_source=" << c.crypto_online_source << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "watch_interval_sec=" << c.watch_interval_sec << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "profit_calc_enabled=" << (c.profit_calc_enabled ? "true" : "false") << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "online_crypto_enabled=" << (c.online_crypto_enabled ? "true" : "false") << "\n";
// печатаем понятный текст чтобы было легче следить за запуском
    std::cout << "no_fixtures=" << (c.no_fixtures ? "true" : "false") << "\n";
// Конец текущего блока.
}

// этой строкой фиксируем промежуточный результат
} // namespace

// main: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Рабочий шаг в общей цепочке.
int main(int argc, char** argv) {
// в этом блоке аккуратно ловим возможные исключения
    try {
// Фиксируем ожидаемое значение.
        const std::string cmd = (argc >= 2) ? argv[1] : "run";
// проверяем условие чтобы отсеять неподходящий сценарий
        if (cmd != "run" && cmd != "watch" && cmd != "config") {
// print_usage: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            print_usage();
// Возвращаем вычисленное значение.
            return 1;
// Конец текущего блока.
        }

// Значение сохраняется для следующих шагов.
        std::string config_path = "config/app.conf";
// Подготовка к следующему шагу.
        std::optional<int> interval_override;
// Подготовка к следующему шагу.
        std::optional<bool> online_crypto_override;
// Подготовка к следующему шагу.
        std::optional<bool> no_fixtures_override;
// Подготовка к следующему шагу.
        std::optional<double> start_capital_override;

        // Пояснение к алгоритму.
// Обходим все элементы.
        for (int i = 2; i < argc; ++i) {
// Фиксируем ожидаемое значение.
            const std::string arg = argv[i];
// проверяем условие чтобы отсеять неподходящий сценарий
            if (arg == "--config" && i + 1 < argc) {
// Значение сохраняется для следующих шагов.
                config_path = argv[++i];
// Значение сохраняется для следующих шагов.
            } else if (arg == "--interval-sec" && i + 1 < argc) {
// stoi: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
                interval_override = std::stoi(argv[++i]);
// Значение сохраняется для следующих шагов.
            } else if (arg == "--online-crypto" && i + 1 < argc) {
// parse_bool_cli: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
                online_crypto_override = parse_bool_cli(argv[++i]);
// Значение сохраняется для следующих шагов.
            } else if (arg == "--no-fixtures" && i + 1 < argc) {
// parse_bool_cli: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
                no_fixtures_override = parse_bool_cli(argv[++i]);
// Значение сохраняется для следующих шагов.
            } else if (arg == "--start-capital" && i + 1 < argc) {
// stod: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
                start_capital_override = std::stod(argv[++i]);
// Значение сохраняется для следующих шагов.
            } else if (arg.rfind("--", 0) == 0) {
// бросаем исключение чтобы явно подсветить ошибочный кейс
                throw am::ConfigError("Unknown CLI option: " + arg);
// тут закрываем небольшой кусок общей задачи
            } else {
// бросаем исключение чтобы явно подсветить ошибочный кейс
                throw am::ConfigError("Unexpected positional argument: " + arg);
// Конец текущего блока.
            }
// Конец текущего блока.
        }

// Значение сохраняется для следующих шагов.
        std::unique_ptr<am::IConfigManager> config = std::make_unique<am::ConfigManager>(config_path);
// load: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
        config->load();

// Подготовка к следующему шагу.
        EffectiveConfig ec;
// Значение сохраняется для следующих шагов.
        ec.config_path = config_path;
// get_string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_quotes_csv = config->get_string("crypto_quotes_csv").value_or("");
// get_string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_fees_csv = config->get_string("crypto_fees_csv").value_or("");
// get_string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_output_csv = config->get_string("crypto_output_csv").value_or("crypto_opportunities.csv");
// get_string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.profit_output_csv = config->get_string("profit_output_csv").value_or("crypto_profit.csv");
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_symbol = am::str::upper_trim_copy(config->get_string("crypto_symbol").value_or("BTCUSD"));
// get_string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_symbols_csv = config->get_string("crypto_symbols").value_or(ec.crypto_symbol);
// get_string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_exchanges_csv = config->get_string("crypto_exchanges").value_or("BINANCE,KRAKEN,BITSTAMP,FXPRO,OANDA,PEPPERSTONE");
// upper_trim_copy: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_online_source = am::str::upper_trim_copy(config->get_string("crypto_online_source").value_or("TRADINGVIEW"));
// get_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_default_fee_bps = config->get_double("crypto_default_fee_bps").value_or(10.0);
// get_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_min_net_spread = config->get_double("crypto_min_net_spread").value_or(0.0);
// get_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_min_net_pct = config->get_double("crypto_min_net_pct").value_or(0.0);
// get_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.start_capital = config->get_double("start_capital").value_or(1000.0);
// get_double: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.watch_interval_sec = static_cast<int>(config->get_double("watch_interval_sec").value_or(5.0));
// get_bool: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.profit_calc_enabled = config->get_bool("profit_calc_enabled").value_or(false);
// get_bool: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.online_crypto_enabled = config->get_bool("online_crypto_enabled").value_or(false);
// get_bool: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.no_fixtures = config->get_bool("no_fixtures").value_or(false);

// проверяем условие чтобы отсеять неподходящий сценарий
        if (interval_override.has_value()) ec.watch_interval_sec = *interval_override;
// проверяем условие чтобы отсеять неподходящий сценарий
        if (online_crypto_override.has_value()) ec.online_crypto_enabled = *online_crypto_override;
// проверяем условие чтобы отсеять неподходящий сценарий
        if (no_fixtures_override.has_value()) ec.no_fixtures = *no_fixtures_override;
// проверяем условие чтобы отсеять неподходящий сценарий
        if (start_capital_override.has_value()) ec.start_capital = *start_capital_override;
// проверяем условие чтобы отсеять неподходящий сценарий
        if (ec.start_capital < 0.0) throw am::ConfigError("start_capital cannot be negative");

// resolve_from_config: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_quotes_csv = resolve_from_config(ec.config_path, ec.crypto_quotes_csv);
// resolve_from_config: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_fees_csv = resolve_from_config(ec.config_path, ec.crypto_fees_csv);
// resolve_from_config: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.crypto_output_csv = resolve_from_config(ec.config_path, ec.crypto_output_csv);
// resolve_from_config: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
        ec.profit_output_csv = resolve_from_config(ec.config_path, ec.profit_output_csv);

// проверяем условие чтобы отсеять неподходящий сценарий
        if (ec.no_fixtures) {
// проверяем условие чтобы отсеять неподходящий сценарий
            if (is_fixture_or_test_path(ec.crypto_quotes_csv)) ec.crypto_quotes_csv.clear();
// проверяем условие чтобы отсеять неподходящий сценарий
            if (is_fixture_or_test_path(ec.crypto_fees_csv)) ec.crypto_fees_csv.clear();
// Конец текущего блока.
        }

// проверяем условие чтобы отсеять неподходящий сценарий
        if (cmd == "config") {
// print_effective_config: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            print_effective_config(ec);
// Возвращаем вычисленное значение.
            return 0;
// Конец текущего блока.
        }

// печатаем понятный текст чтобы было легче следить за запуском
        std::cout << "[CRYPTO_ONLY] ArbitrageMonitor is running in crypto-arbitrage mode only.\n";

// path: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const auto out_path = std::filesystem::path(ec.crypto_output_csv);
// parent_path: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
        const auto out_dir = out_path.parent_path().empty() ? std::filesystem::path(".") : out_path.parent_path();
// create_directories: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
        std::filesystem::create_directories(out_dir);
// Значение сохраняется для следующих шагов.
        double rolling_capital = ec.start_capital;
// Значение сохраняется для следующих шагов.
        std::shared_ptr<am::IArbitrageEngine> engine = std::make_shared<am::CryptoArbitrageEngine>();
// открываем новый блок логики
        {
// reset_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            engine->reset_opportunities_csv(ec.crypto_output_csv);
// проверяем условие чтобы отсеять неподходящий сценарий
            if (ec.profit_calc_enabled) {
// path: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
                const auto profit_path = std::filesystem::path(ec.profit_output_csv);
// parent_path: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
                const auto profit_dir = profit_path.parent_path().empty() ? std::filesystem::path(".") : profit_path.parent_path();
// create_directories: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
                std::filesystem::create_directories(profit_dir);
// reset_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
                engine->reset_profit_csv(ec.profit_output_csv);
// Конец текущего блока.
            }
// Конец текущего блока.
        }

        // Пояснение к текущему шагу.
// Значение сохраняется для следующих шагов.
        auto execute_once = [&]() {
// load_fees_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
            const auto fees_map = engine->load_fees_csv(ec.crypto_fees_csv);
// parse_csv_list: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
            auto selected_symbols = parse_csv_list(ec.crypto_symbols_csv);
// проверяем условие чтобы отсеять неподходящий сценарий
            if (selected_symbols.empty()) selected_symbols = parse_csv_list(ec.crypto_symbol);
// проверяем условие чтобы отсеять неподходящий сценарий
            if (selected_symbols.empty()) {
// бросаем исключение чтобы явно подсветить ошибочный кейс
                throw am::ConfigError("At least one symbol must be provided via crypto_symbols or crypto_symbol");
// Конец текущего блока.
            }
// selected_symbol_set: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            std::unordered_set<std::string> selected_symbol_set(selected_symbols.begin(), selected_symbols.end());

// detect_quotes_source: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
            const QuotesSource source = detect_quotes_source(ec);
// visit: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
            auto quotes = std::visit([&](const auto& source_mode) -> std::vector<am::CryptoQuote> {
// даём удобное короткое имя для следующего кода
                using SourceMode = std::decay_t<decltype(source_mode)>;
// Поддерживаем линейную логику.
                if constexpr (std::is_same_v<SourceMode, OfflineQuotesSource>) {
// проверяем условие чтобы отсеять неподходящий сценарий
                    if (ec.crypto_quotes_csv.empty()) {
// бросаем исключение чтобы явно подсветить ошибочный кейс
                        throw am::ConfigError("crypto_quotes_csv is required when online_crypto_enabled=false");
// Конец текущего блока.
                    }
// проверяем условие чтобы отсеять неподходящий сценарий
                    if (!std::filesystem::exists(ec.crypto_quotes_csv)) {
// бросаем исключение чтобы явно подсветить ошибочный кейс
                        throw am::CsvError("Crypto quotes file not found: " + ec.crypto_quotes_csv);
// Конец текущего блока.
                    }
// Возвращаем вычисленное значение.
                    return engine->load_quotes_csv(ec.crypto_quotes_csv, fees_map, ec.crypto_default_fee_bps);
// Конец текущего блока.
                }
// Значение сохраняется для следующих шагов.
                std::unique_ptr<am::IMarketDataConnectors> online = std::make_unique<am::MarketDataConnectors>();
// Рабочий шаг в общей цепочке.
                if constexpr (std::is_same_v<SourceMode, TradingViewQuotesSource>) {
// parse_csv_set: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
                    const auto allowed = parse_csv_set(ec.crypto_exchanges_csv);
// Возвращаем вычисленное значение.
                    return online->fetch_tradingview_quotes(fees_map, ec.crypto_default_fee_bps, selected_symbols, allowed);
// Конец текущего блока.
                }
// Возвращаем вычисленное значение.
                return online->fetch_btcusd_quotes(fees_map, ec.crypto_default_fee_bps);
// Подготовка к следующему шагу.
            }, source);
// erase: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
            quotes.erase(std::remove_if(quotes.begin(), quotes.end(), [&](const am::CryptoQuote& q) {
// Возвращаем вычисленное значение.
                return selected_symbol_set.count(am::str::upper_trim_copy(q.symbol)) == 0;
// end: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            }), quotes.end());

// Подготовка к следующему шагу.
            am::CryptoMonitorConfig ccfg;
// size: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
            ccfg.symbol_filter = (selected_symbols.size() == 1) ? selected_symbols.front() : "";
// parse_csv_set: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
            ccfg.allowed_exchanges = parse_csv_set(ec.crypto_exchanges_csv);
// Значение сохраняется для следующих шагов.
            ccfg.default_fee_bps = ec.crypto_default_fee_bps;
// Значение сохраняется для следующих шагов.
            ccfg.min_net_spread = ec.crypto_min_net_spread;
// Значение сохраняется для следующих шагов.
            ccfg.min_net_pct = ec.crypto_min_net_pct;

// find_opportunities: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
            const auto opps = engine->find_opportunities(quotes, ccfg);
// utc_now_iso8601: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
            const std::string observed_at = utc_now_iso8601();
// append_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
            engine->append_opportunities_csv(ec.crypto_output_csv, observed_at, opps);
// проверяем условие чтобы отсеять неподходящий сценарий
            if (ec.profit_calc_enabled) {
// Фиксируем ожидаемое значение.
                const double before = rolling_capital;
// append_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
                rolling_capital = engine->append_profit_csv(ec.profit_output_csv, observed_at, rolling_capital, opps);
// печатаем понятный текст чтобы было легче следить за запуском
                std::cout << "Profit calc enabled. Capital: " << before << " -> " << rolling_capital
// доклеиваем следующую часть данных в выходной поток
                          << ". Appended profit report: " << ec.profit_output_csv << "\n";
// Конец текущего блока.
            }

// проверяем условие чтобы отсеять неподходящий сценарий
            if (quotes.size() < 2) {
// iteration: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// печатаем понятный текст чтобы было легче следить за запуском
                std::cerr << "Crypto monitor warning: less than two exchanges available in this iteration ("
// size: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// доклеиваем следующую часть данных в выходной поток
                          << quotes.size() << ")\n";
// Конец текущего блока.
            }
// Обходим все элементы.
            for (const auto& o : opps) {
// печатаем понятный текст чтобы было легче следить за запуском
                std::cout << "[CRYPTO_ARB] " << o.symbol
// доклеиваем следующую часть данных в выходной поток
                          << " buy@" << o.buy_exchange << " ask=" << o.buy_ask
// доклеиваем следующую часть данных в выходной поток
                          << " sell@" << o.sell_exchange << " bid=" << o.sell_bid
// доклеиваем следующую часть данных в выходной поток
                          << " net=" << o.net_spread
// доклеиваем следующую часть данных в выходной поток
                          << " net_pct=" << o.net_pct << "\n";
// Конец текущего блока.
            }
// size: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// печатаем понятный текст чтобы было легче следить за запуском
            std::cout << "Done. Quotes=" << quotes.size() << ", opportunities=" << opps.size()
// доклеиваем следующую часть данных в выходной поток
                      << ". Appended report: " << ec.crypto_output_csv << "\n";
// Конец объявления типа.
        };

        // Пояснение к следующему шагу.
// проверяем условие чтобы отсеять неподходящий сценарий
        if (cmd == "watch") {
// Повторяем, пока условие истинно.
            while (true) {
// в этом блоке аккуратно ловим возможные исключения
                try {
// execute_once: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
                    execute_once();
// Шаг текущего сценария.
                } catch (const std::exception& e) {
// what: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// печатаем понятный текст чтобы было легче следить за запуском
                    std::cerr << "Watch iteration failed: " << e.what() << "\n";
// Конец текущего блока.
                }
// sleep_for: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
                std::this_thread::sleep_for(std::chrono::seconds(std::max(1, ec.watch_interval_sec)));
// Конец текущего блока.
            }
// Конец текущего блока.
        }

// execute_once: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
        execute_once();
// Возвращаем вычисленное значение.
        return 0;
// Рабочий шаг в общей цепочке.
    } catch (const am::AmException& e) {
// what: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// печатаем понятный текст чтобы было легче следить за запуском
        std::cerr << "AM error: " << e.what() << "\n";
// Возвращаем вычисленное значение.
        return 2;
// Поддерживаем линейную логику.
    } catch (const std::exception& e) {
// what: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// печатаем понятный текст чтобы было легче следить за запуском
        std::cerr << "Unexpected error: " << e.what() << "\n";
// Возвращаем вычисленное значение.
        return 3;
// Конец текущего блока.
    }
// Конец текущего блока.
}


