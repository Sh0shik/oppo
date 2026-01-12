// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#ifndef CURRENCY_RATE_H_
#define CURRENCY_RATE_H_

#include <iostream>
#include <stdexcept>
#include <string>

class CurrencyRateException : public std::runtime_error {
 public:
  explicit CurrencyRateException(const std::string& message)
      : std::runtime_error(message) {}
};

class InvalidFormatException : public CurrencyRateException {
 public:
  explicit InvalidFormatException(const std::string& message)
      : CurrencyRateException("Неверный формат: " + message) {}
};

class InvalidDateException : public CurrencyRateException {
 public:
  explicit InvalidDateException(const std::string& message)
      : CurrencyRateException("Неверная дата: " + message) {}
};

class InvalidRateException : public CurrencyRateException {
 public:
  explicit InvalidRateException(const std::string& message)
      : CurrencyRateException("Неверный курс: " + message) {}
};

class InvalidCurrencyException : public CurrencyRateException {
 public:
  explicit InvalidCurrencyException(const std::string& message)
      : CurrencyRateException("Неверное название валюты: " + message) {}
};

class CurrencyRate {
 public:
  CurrencyRate(const std::string& currency1, const std::string& currency2,
               double rate, const std::string& date);

  const std::string& currency1() const { return currency1_; }
  const std::string& currency2() const { return currency2_; }
  double rate() const { return rate_; }
  const std::string& date() const { return date_; }

  std::string ToString() const;
  std::string ToFileString() const;

  bool is_future_date() const;
  static bool is_valid_date(int year, int month, int day);

  bool operator<(const CurrencyRate& other) const;
  bool operator==(const CurrencyRate& other) const;

 private:
  std::string currency1_;
  std::string currency2_;
  double rate_;
  std::string date_;

  void Validate() const;
};

std::ostream& operator<<(std::ostream& os, const CurrencyRate& rate);

#endif  // CURRENCY_RATE_H_