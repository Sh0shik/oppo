// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#ifndef CURRENCY_RATE_VALIDATOR_H_
#define CURRENCY_RATE_VALIDATOR_H_

#include <string>

class CurrencyRateValidator {
public:
  static bool IsValidCurrencyName(const std::string& name);
  static bool IsValidRate(double rate);
  static bool IsValidDate(const std::string& date);

  static void ValidateCurrencyName(const std::string& name);
  static void ValidateRate(double rate);
  static void ValidateDate(const std::string& date);

private:
  static bool IsLeapYear(int year);
  static int GetDaysInMonth(int year, int month);
};

#endif  // CURRENCY_RATE_VALIDATOR_H_