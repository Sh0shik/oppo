// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#ifndef CURRENCY_RATE_VALIDATOR_H_
#define CURRENCY_RATE_VALIDATOR_H_

#include "currency_rate.h"

class CurrencyRateValidator {
public:
  static bool is_valid_currency_name(const std::string& name);
  static bool is_valid_rate(double rate);
  static bool is_valid_date(const std::string& date);

  static void validate_currency_name(const std::string& name);
  static void validate_rate(double rate);
  static void validate_date(const std::string& date);

private:
  static bool is_leap_year(int year);
  static int get_days_in_month(int year, int month);
};

#endif  // CURRENCY_RATE_VALIDATOR_H_