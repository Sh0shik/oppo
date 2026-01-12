// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#ifndef CURRENCY_RATE_H_
#define CURRENCY_RATE_H_

#include <iostream>
#include <string>

class CurrencyRateException : public std::exception {
public:
    explicit CurrencyRateException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

class InvalidCurrencyException : public CurrencyRateException {
public:
    explicit InvalidCurrencyException(const std::string& message) : CurrencyRateException(message) {}
};

class InvalidRateException : public CurrencyRateException {
public:
    explicit InvalidRateException(const std::string& message) : CurrencyRateException(message) {}
};

class InvalidDateException : public CurrencyRateException {
public:
    explicit InvalidDateException(const std::string& message) : CurrencyRateException(message) {}
};

class InvalidFormatException : public CurrencyRateException {
public:
    explicit InvalidFormatException(const std::string& message) : CurrencyRateException(message) {}
};

class CurrencyRate {
public:
    CurrencyRate(const std::string& currency1, const std::string& currency2,
                 double rate, const std::string& date);

    // Геттеры
    std::string currency1() const { return currency1_; }
    std::string currency2() const { return currency2_; }
    double rate() const { return rate_; }
    std::string date() const { return date_; }

    // Форматирование
    std::string ToString() const;
    std::string ToFileString() const;

    // Валидация
    void Validate() const;
    bool is_future_date() const;
    static bool is_valid_date(int year, int month, int day);

    // Операторы сравнения
    bool operator<(const CurrencyRate& other) const;
    bool operator==(const CurrencyRate& other) const;

private:
    std::string currency1_;
    std::string currency2_;
    double rate_;
    std::string date_;
};

std::ostream& operator<<(std::ostream& os, const CurrencyRate& rate);

#endif  // CURRENCY_RATE_H_