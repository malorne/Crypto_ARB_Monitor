// Защита от повторного включения заголовка.
#pragma once
// Подключаем stdexcept.
#include <stdexcept>
// Подключаем string.
#include <string>

// Пространство имен модуля.
namespace am {

// Пояснение к алгоритму.
// AmException: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Продолжение общей логики.
class AmException : public std::runtime_error {
// Публичный API.
public:
// AmException: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
    explicit AmException(const std::string& msg) : std::runtime_error(msg) {}
// Конец объявления типа.
};

// ConfigError: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Технический шаг.
class ConfigError : public AmException {
// Публичный API.
public:
// ConfigError: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// этой строкой фиксируем промежуточный результат
    explicit ConfigError(const std::string& msg) : AmException("ConfigError: " + msg) {}
// Конец объявления типа.
};

// CsvError: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Продолжение общей логики.
class CsvError : public AmException {
// Публичный API.
public:
// CsvError: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Поддерживаем линейную логику.
    explicit CsvError(const std::string& msg) : AmException("CsvError: " + msg) {}
// Конец объявления типа.
};

// DataValidationError: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Продолжение общей логики.
class DataValidationError : public AmException {
// Публичный API.
public:
// DataValidationError: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// Технический шаг.
    explicit DataValidationError(const std::string& msg) : AmException("DataValidationError: " + msg) {}
// Конец объявления типа.
};

// CalculationError: хранит данные и связанную логику.
// Единый контракт для расширения модуля.
// Подготовка данных к следующему шагу.
class CalculationError : public AmException {
// Публичный API.
public:
// CalculationError: ключевой шаг текущего сценария.
// Путь данных: вход -> проверки -> результат.
// тут закрываем небольшой кусок общей задачи
    explicit CalculationError(const std::string& msg) : AmException("CalculationError: " + msg) {}
// Конец объявления типа.
};

// Поддерживаем линейную логику.
} // закрываем пространство имен am


