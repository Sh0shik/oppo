// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#include "currency_rate.h"
#include "currency_rate_validator.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

using std::endl;
using std::invalid_argument;
using std::isdigit;
using std::localtime;
using std::ostringstream;
using std::setprecision;
using std::setw;
using std::string;
using std::time;
using std::time_t;
using std::tm;

namespace {

bool IsAllDigits(const string& str) {
  for (char c : str) {
    if (!isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
  }
  return true;
}

bool IsLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int GetDaysInMonth(int year, int month) {
  static const int kDaysInMonth[] = {31, 28, 31, 30, 31, 30,
                                     31, 31, 30, 31, 30, 31};

  if (month < 1 || month > 12) {
    return 0;
  }

  if (month == 2 && IsLeapYear(year)) {
    return 29;
  }

  return kDaysInMonth[month - 1];
}

}  // namespace

CurrencyRate::CurrencyRate(const string& currency1, const string& currency2,
                           double rate, const string& date)
    : currency1_(currency1), currency2_(currency2), rate_(rate), date_(date) {
  Validate();
}

void CurrencyRate::Validate() const {
  CurrencyRateValidator::validate_currency_name(currency1_);
  CurrencyRateValidator::validate_currency_name(currency2_);
  CurrencyRateValidator::validate_rate(rate_);
  CurrencyRateValidator::validate_date(date_);

  if (currency1_ == currency2_) {
    throw CurrencyRateException("Валюты не могут быть одинаковыми");
  }
}

string CurrencyRate::ToString() const {
  ostringstream oss;
  oss << std::fixed << setprecision(4)
      << "Валюта 1: " << currency1_ << endl
      << "Валюта 2: " << currency2_ << endl
      << "Курс: " << rate_ << endl
      << "Дата: " << date_;
  return oss.str();
}

string CurrencyRate::ToFileString() const {
  ostringstream oss;

  if (currency1_.find(' ') != string::npos) {
    oss << "\"" << currency1_ << "\" ";
  } else {
    oss << currency1_ << " ";
  }

  if (currency2_.find(' ') != string::npos) {
    oss << "\"" << currency2_ << "\" ";
  } else {
    oss << currency2_ << " ";
  }

  oss << std::fixed << setprecision(4) << rate_ << " " << date_;
  return oss.str();
}

bool CurrencyRate::operator<(const CurrencyRate& other) const {
  if (date_ != other.date_) {
    return date_ < other.date_;
  }
  if (currency1_ != other.currency1_) {
    return currency1_ < other.currency1_;
  }
  return currency2_ < other.currency2_;
}

bool CurrencyRate::operator==(const CurrencyRate& other) const {
  return currency1_ == other.currency1_ &&
         currency2_ == other.currency2_ &&
         rate_ == other.rate_ &&
         date_ == other.date_;
}

bool CurrencyRate::is_future_date() const {
  if (date_.length() != 10 || date_[4] != '.' || date_[7] != '.') {
    return false;
  }

  string year_str = date_.substr(0, 4);
  string month_str = date_.substr(5, 2);
  string day_str = date_.substr(8, 2);

  if (!IsAllDigits(year_str) || !IsAllDigits(month_str) ||
      !IsAllDigits(day_str)) {
    return false;
  }

  int year = stoi(year_str);
  int month = stoi(month_str);
  int day = stoi(day_str);

  time_t now = time(nullptr);
  tm* local_time = localtime(&now);

  int current_year = local_time->tm_year + 1900;
  int current_month = local_time->tm_mon + 1;
  int current_day = local_time->tm_mday;

  if (year > current_year) {
    return true;
  } else if (year == current_year) {
    if (month > current_month) {
      return true;
    } else if (month == current_month) {
      return day > current_day;
    }
  }

  return false;
}

bool CurrencyRate::is_valid_date(int year, int month, int day) {
  if (year < 1900 || year > 2100) {
    return false;
  }
  if (month < 1 || month > 12) {
    return false;
  }

  int max_days = GetDaysInMonth(year, month);
  return (day >= 1 && day <= max_days);
}

std::ostream& operator<<(std::ostream& os, const CurrencyRate& rate) {
  os << rate.ToString();
  return os;
}