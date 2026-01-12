// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#include "currency_rate_validator.h"
#include "currency_rate.h"

#include <ctime>
#include <regex>
#include <cctype>

using std::invalid_argument;
using std::isdigit;
using std::isalnum;
using std::localtime;
using std::regex;
using std::regex_match;
using std::stoi;
using std::string;
using std::time;
using std::time_t;
using std::tm;
using std::to_string;

bool CurrencyRateValidator::is_valid_currency_name(const string& name) {
  if (name.empty() || name.length() > 50) {
    return false;
  }

  for (char c : name) {
    unsigned char uc = static_cast<unsigned char>(c);
    if (!isalnum(uc) && c != ' ' && c != '-' && c != '(' && c != ')' &&
        c != '/') {
      return false;
    }
  }

  return true;
}

bool CurrencyRateValidator::is_valid_rate(double rate) {
  return rate > 0 && rate < 1000000;
}

bool CurrencyRateValidator::is_valid_date(const string& date) {
  static const regex kDatePattern(R"(^\d{4}\.\d{2}\.\d{2}$)");

  if (!regex_match(date, kDatePattern)) {
    return false;
  }

  string year_str = date.substr(0, 4);
  string month_str = date.substr(5, 2);
  string day_str = date.substr(8, 2);

  for (char c : year_str + month_str + day_str) {
    if (!isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
  }

  try {
    int year = stoi(year_str);
    int month = stoi(month_str);
    int day = stoi(day_str);

    if (year < 1900 || year > 2100) {
      return false;
    }

    if (month < 1 || month > 12) {
      return false;
    }

    int days_in_month = get_days_in_month(year, month);
    if (day < 1 || day > days_in_month) {
      return false;
    }

    time_t now = time(nullptr);
    tm* local_time = localtime(&now);

    int current_year = local_time->tm_year + 1900;
    int current_month = local_time->tm_mon + 1;
    int current_day = local_time->tm_mday;

    if (year > current_year) {
      return false;
    } else if (year == current_year) {
      if (month > current_month) {
        return false;
      } else if (month == current_month) {
        if (day > current_day) {
          return false;
        }
      }
    }

  } catch (...) {
    return false;
  }

  return true;
}

void CurrencyRateValidator::validate_currency_name(const string& name) {
  if (!is_valid_currency_name(name)) {
    throw InvalidCurrencyException("Название валюты '" + name +
        "' содержит недопустимые символы или неверную длину");
  }
}

void CurrencyRateValidator::validate_rate(double rate) {
  if (!is_valid_rate(rate)) {
    throw InvalidRateException("Курс " + to_string(rate) +
        " выходит за допустимые пределы (0...1000000)");
  }
}

void CurrencyRateValidator::validate_date(const string& date) {
  if (!is_valid_date(date)) {
    throw InvalidDateException("Дата '" + date +
        "' имеет неверный формат, недопустимое значение "
        "или находится в будущем");
  }
}

bool CurrencyRateValidator::is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int CurrencyRateValidator::get_days_in_month(int year, int month) {
  static const int kDaysInMonth[] = {31, 28, 31, 30, 31, 30,
                                     31, 31, 30, 31, 30, 31};

  if (month < 1 || month > 12) {
    return 0;
  }

  if (month == 2 && is_leap_year(year)) {
    return 29;
  }

  return kDaysInMonth[month - 1];
}