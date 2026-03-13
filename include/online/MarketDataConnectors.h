// Защита от повторного включения заголовка.
#pragma once

// Подключаем functional.
#include <functional>
// Подключаем optional.
#include <optional>
// Подключаем string.
#include <string>
// Подключаем unordered_map.
#include <unordered_map>
// Подключаем unordered_set.
#include <unordered_set>
// Подключаем vector.
#include <vector>

// Подключаем crypto/CryptoArbitrageEngine.h.
#include "crypto/CryptoArbitrageEngine.h"

// Пространство имен модуля.
namespace am {

// Пояснение к алгоритму.
// TradingViewRowCandidate: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Продолжение общей логики.
struct TradingViewRowCandidate {
// Подготовка к следующему шагу.
    std::string exchange;
// Подготовка к следующему шагу.
    std::string symbol;
// Подготовка к следующему шагу.
    std::optional<double> bid;
// Подготовка к следующему шагу.
    std::optional<double> ask;
// Подготовка к следующему шагу.
    std::optional<double> close;
// Конец объявления типа.
};

// IMarketDataConnectors: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// тут закрываем небольшой кусок общей задачи
class IMarketDataConnectors {
// Публичный API.
public:
// ~IMarketDataConnectors: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    virtual ~IMarketDataConnectors() = default;

// fetch_btcusd_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Рабочий шаг в общей цепочке.
    virtual std::vector<CryptoQuote> fetch_btcusd_quotes(
// Фиксируем ожидаемое значение.
        const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Значение сохраняется для следующих шагов.
        double default_fee_bps) const = 0;
// fetch_tradingview_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Рабочий шаг в общей цепочке.
    virtual std::vector<CryptoQuote> fetch_tradingview_quotes(
// Фиксируем ожидаемое значение.
        const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Продолжение общей логики.
        double default_fee_bps,
// Фиксируем ожидаемое значение.
        const std::vector<std::string>& symbols,
// Фиксируем ожидаемое значение.
        const std::unordered_set<std::string>& allowed_exchanges) const = 0;
// Конец объявления типа.
};

// MarketDataConnectors: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Шаг текущего сценария.
class MarketDataConnectors final : public IMarketDataConnectors {
// Публичный API.
public:
// string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// даём удобное короткое имя для следующего кода
    using HttpGetFn = std::function<std::string(const std::string&)>;
// string: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// даём удобное короткое имя для следующего кода
    using HttpPostJsonFn = std::function<std::string(const std::string&, const std::string&)>;

    // Назначение блока.
// MarketDataConnectors: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    MarketDataConnectors();
    // Пояснение к алгоритму.
// MarketDataConnectors: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    explicit MarketDataConnectors(HttpGetFn http_get_fn, HttpPostJsonFn http_post_json_fn);

    // Пояснение к алгоритму.
// parse_binance_book_ticker: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    static std::optional<std::pair<double, double>> parse_binance_book_ticker(const std::string& json);
// parse_kraken_ticker: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    static std::optional<std::pair<double, double>> parse_kraken_ticker(const std::string& json);
// parse_bitstamp_ticker: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    static std::optional<std::pair<double, double>> parse_bitstamp_ticker(const std::string& json);

    // Пояснение к текущему шагу.
// build_tradingview_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Шаг текущего сценария.
    static std::vector<CryptoQuote> build_tradingview_quotes(
// Фиксируем ожидаемое значение.
        const std::vector<TradingViewRowCandidate>& rows,
// Фиксируем ожидаемое значение.
        const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Продолжение общей логики.
        double default_fee_bps,
// Фиксируем ожидаемое значение.
        const std::vector<std::string>& symbols,
// Фиксируем ожидаемое значение.
        const std::unordered_set<std::string>& allowed_exchanges);

    // Пояснение к алгоритму.
// fetch_btcusd_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Поддерживаем линейную логику.
    std::vector<CryptoQuote> fetch_btcusd_quotes(
// Фиксируем ожидаемое значение.
        const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Подготовка к следующему шагу.
        double default_fee_bps) const override;
// fetch_tradingview_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Поддерживаем линейную логику.
    std::vector<CryptoQuote> fetch_tradingview_quotes(
// Фиксируем ожидаемое значение.
        const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Подготовка данных к следующему шагу.
        double default_fee_bps,
// Фиксируем ожидаемое значение.
        const std::vector<std::string>& symbols,
// Фиксируем ожидаемое значение.
        const std::unordered_set<std::string>& allowed_exchanges) const override;

// Внутреннее состояние.
private:
// shell_escape_single_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    static std::string shell_escape_single_quotes(const std::string& s);
// extract_json_number_field: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    static std::optional<double> extract_json_number_field(const std::string& json, const std::string& field);
// curl_http_post_json: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    static std::string curl_http_post_json(const std::string& url, const std::string& json_body);
// curl_http_get: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    static std::string curl_http_get(const std::string& url);
// http_post_json: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::string http_post_json(const std::string& url, const std::string& json_body) const;
// http_get: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::string http_get(const std::string& url) const;

// Подготовка к следующему шагу.
    HttpGetFn http_get_fn_;
// Подготовка к следующему шагу.
    HttpPostJsonFn http_post_json_fn_;
// Конец объявления типа.
};

// этой строкой фиксируем промежуточный результат
} // закрываем пространство имен am


