// Защита от повторного включения заголовка.
#pragma once

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

// Пространство имен модуля.
namespace am {

// Пояснение к текущему шагу.
// CryptoQuote: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Рабочий шаг в общей цепочке.
struct CryptoQuote {
// Подготовка к следующему шагу.
    std::string exchange;
// Подготовка к следующему шагу.
    std::string symbol;
// Значение сохраняется для следующих шагов.
    double bid = 0.0;
// Значение сохраняется для следующих шагов.
    double ask = 0.0;
// Значение сохраняется для следующих шагов.
    double fee_bps = 0.0;
// Конец объявления типа.
};

// CryptoOpportunity: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Шаг текущего сценария.
struct CryptoOpportunity {
// Подготовка к следующему шагу.
    std::string symbol;
// Подготовка к следующему шагу.
    std::string buy_exchange;
// Подготовка к следующему шагу.
    std::string sell_exchange;
// Значение сохраняется для следующих шагов.
    double buy_ask = 0.0;
// Значение сохраняется для следующих шагов.
    double sell_bid = 0.0;
// Значение сохраняется для следующих шагов.
    double gross_spread = 0.0;
// Значение сохраняется для следующих шагов.
    double net_spread = 0.0;
// Значение сохраняется для следующих шагов.
    double net_pct = 0.0;
// Конец объявления типа.
};

// CryptoMonitorConfig: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// этой строкой фиксируем промежуточный результат
struct CryptoMonitorConfig {
// Значение сохраняется для следующих шагов.
    std::string symbol_filter = "BTCUSD";
// Подготовка к следующему шагу.
    std::unordered_set<std::string> allowed_exchanges;
// Значение сохраняется для следующих шагов.
    double default_fee_bps = 10.0;
// Значение сохраняется для следующих шагов.
    double min_net_spread = 0.0;
// Значение сохраняется для следующих шагов.
    double min_net_pct = 0.0;
// Конец объявления типа.
};

// IArbitrageEngine: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Продолжение общей логики.
class IArbitrageEngine {
// Публичный API.
public:
// ~IArbitrageEngine: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    virtual ~IArbitrageEngine() = default;

// load_quotes_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
    virtual std::vector<CryptoQuote> load_quotes_csv(
// Фиксируем ожидаемое значение.
        const std::string& csv_path,
// Фиксируем ожидаемое значение.
        const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Значение сохраняется для следующих шагов.
        double default_fee_bps) const = 0;

// load_fees_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    virtual std::unordered_map<std::string, double> load_fees_csv(const std::string& csv_path) const = 0;

// find_opportunities: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
    virtual std::vector<CryptoOpportunity> find_opportunities(
// Фиксируем ожидаемое значение.
        const std::vector<CryptoQuote>& quotes,
// Фиксируем ожидаемое значение.
        const CryptoMonitorConfig& cfg) const = 0;

    // Пояснение к алгоритму.
// reset_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    virtual void reset_opportunities_csv(const std::string& path) const = 0;
// append_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Шаг текущего сценария.
    virtual void append_opportunities_csv(
// Фиксируем ожидаемое значение.
        const std::string& path,
// Фиксируем ожидаемое значение.
        const std::string& observed_at,
// Фиксируем ожидаемое значение.
        const std::vector<CryptoOpportunity>& opps) const = 0;
// reset_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Значение сохраняется для следующих шагов.
    virtual void reset_profit_csv(const std::string& path) const = 0;
// append_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Шаг текущего сценария.
    virtual double append_profit_csv(
// Фиксируем ожидаемое значение.
        const std::string& path,
// Фиксируем ожидаемое значение.
        const std::string& observed_at,
// Шаг текущего сценария.
        double capital_before,
// Фиксируем ожидаемое значение.
        const std::vector<CryptoOpportunity>& opps) const = 0;

// write_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Рабочий шаг в общей цепочке.
    virtual void write_opportunities_csv(
// Фиксируем ожидаемое значение.
        const std::string& path,
// Фиксируем ожидаемое значение.
        const std::string& observed_at,
// Фиксируем ожидаемое значение.
        const std::vector<CryptoOpportunity>& opps) const = 0;
// Конец объявления типа.
};

// CryptoArbitrageEngine: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Шаг текущего сценария.
class CryptoArbitrageEngine final : public IArbitrageEngine {
// Публичный API.
public:
// load_quotes_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
    std::vector<CryptoQuote> load_quotes_csv(
// Фиксируем ожидаемое значение.
        const std::string& csv_path,
// Фиксируем ожидаемое значение.
        const std::unordered_map<std::string, double>& per_exchange_fee_bps,
// Подготовка к следующему шагу.
        double default_fee_bps) const override;

// load_fees_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    std::unordered_map<std::string, double> load_fees_csv(const std::string& csv_path) const override;

// find_opportunities: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
    std::vector<CryptoOpportunity> find_opportunities(
// Фиксируем ожидаемое значение.
        const std::vector<CryptoQuote>& quotes,
// Фиксируем ожидаемое значение.
        const CryptoMonitorConfig& cfg) const override;

// reset_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    void reset_opportunities_csv(const std::string& path) const override;
// append_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Технический шаг.
    void append_opportunities_csv(
// Фиксируем ожидаемое значение.
        const std::string& path,
// Фиксируем ожидаемое значение.
        const std::string& observed_at,
// Фиксируем ожидаемое значение.
        const std::vector<CryptoOpportunity>& opps) const override;
// reset_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    void reset_profit_csv(const std::string& path) const override;
// append_profit_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Технический шаг.
    double append_profit_csv(
// Фиксируем ожидаемое значение.
        const std::string& path,
// Фиксируем ожидаемое значение.
        const std::string& observed_at,
// Технический шаг.
        double capital_before,
// Фиксируем ожидаемое значение.
        const std::vector<CryptoOpportunity>& opps) const override;

// write_opportunities_csv: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Продолжение общей логики.
    void write_opportunities_csv(
// Фиксируем ожидаемое значение.
        const std::string& path,
// Фиксируем ожидаемое значение.
        const std::string& observed_at,
// Фиксируем ожидаемое значение.
        const std::vector<CryptoOpportunity>& opps) const override;
// Конец объявления типа.
};

// Подготовка данных к следующему шагу.
} // закрываем пространство имен am


