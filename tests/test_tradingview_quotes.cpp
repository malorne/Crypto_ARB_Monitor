// Подключаем gtest/gtest.h.
#include <gtest/gtest.h>

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

// Подключаем online/MarketDataConnectors.h.
#include "online/MarketDataConnectors.h"

// test_case: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Проверяем отдельный сценарий.
TEST(TradingViewQuotes, PrefersRowsWithBidAskAndAppliesFeeMap) {
    // Назначение блока.
// Значение сохраняется для следующих шагов.
    std::vector<am::TradingViewRowCandidate> rows = {
// Технический шаг.
        {"BINANCE", "XTZUSD", std::nullopt, std::nullopt, 5.0},
// Шаг текущего сценария.
        {"BINANCE", "XTZUSD", 4.90, 5.10, 5.0},
// Шаг текущего сценария.
        {"KRAKEN", "XTZUSD", 5.00, 5.20, 5.1},
// Конец объявления типа.
    };

// Фиксируем ожидаемое значение.
    const std::unordered_map<std::string, double> fees = {
// Технический шаг.
        {"BINANCE", 7.5},
// Конец объявления типа.
    };

// build_tradingview_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto quotes = am::MarketDataConnectors::build_tradingview_quotes(
// Подготовка к следующему шагу.
        rows, fees, 10.0, {"XTZUSD"}, {"BINANCE"});

// ASSERT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    ASSERT_EQ(quotes.size(), 1u);
// EXPECT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_EQ(quotes[0].exchange, "BINANCE");
// EXPECT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_EQ(quotes[0].symbol, "XTZUSD");
// EXPECT_NEAR: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_NEAR(quotes[0].bid, 4.90, 1e-12);
// EXPECT_NEAR: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_NEAR(quotes[0].ask, 5.10, 1e-12);
// EXPECT_NEAR: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_NEAR(quotes[0].fee_bps, 7.5, 1e-12);
// Конец текущего блока.
}

// test_case: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Проверяем отдельный сценарий.
TEST(TradingViewQuotes, FallsBackToCloseWhenBidAskMissing) {
    // Пояснение к следующему шагу.
// Значение сохраняется для следующих шагов.
    std::vector<am::TradingViewRowCandidate> rows = {
// Технический шаг.
        {"KRAKEN", "APTUSD", std::nullopt, std::nullopt, 12.34},
// Конец объявления типа.
    };

// build_tradingview_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto quotes = am::MarketDataConnectors::build_tradingview_quotes(
// Подготовка к следующему шагу.
        rows, {}, 10.0, {"APTUSD"}, {});

// ASSERT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    ASSERT_EQ(quotes.size(), 1u);
// EXPECT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_EQ(quotes[0].exchange, "KRAKEN");
// EXPECT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_EQ(quotes[0].symbol, "APTUSD");
// EXPECT_NEAR: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_NEAR(quotes[0].bid, 12.34, 1e-12);
// EXPECT_NEAR: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_NEAR(quotes[0].ask, 12.34, 1e-12);
// EXPECT_NEAR: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_NEAR(quotes[0].fee_bps, 10.0, 1e-12);
// Конец текущего блока.
}

// test_case: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Проверяем отдельный сценарий.
TEST(TradingViewQuotes, FiltersOutRowsOutsideRequestedSymbols) {
    // Пояснение к текущему шагу.
// Значение сохраняется для следующих шагов.
    std::vector<am::TradingViewRowCandidate> rows = {
// Технический шаг.
        {"BINANCE", "XTZUSD", 4.90, 5.10, 5.0},
// Продолжение общей логики.
        {"BINANCE", "ARBUSD", 1.00, 1.05, 1.02},
// Конец объявления типа.
    };

// build_tradingview_quotes: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Фиксируем ожидаемое значение.
    const auto quotes = am::MarketDataConnectors::build_tradingview_quotes(
// Подготовка к следующему шагу.
        rows, {}, 10.0, {"ARBUSD"}, {});

// ASSERT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    ASSERT_EQ(quotes.size(), 1u);
// EXPECT_EQ: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Подготовка к следующему шагу.
    EXPECT_EQ(quotes[0].symbol, "ARBUSD");
// Конец текущего блока.
}


