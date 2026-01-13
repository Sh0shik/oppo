// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#ifndef CURRENCY_RATE_PARSER_H_
#define CURRENCY_RATE_PARSER_H_

#include <memory>
#include <regex>

#include "currency_rate.h"

class ICurrencyRateParser {
public:
  virtual ~ICurrencyRateParser() = default;
  virtual CurrencyRate Parse(const std::string& line) const = 0;
  virtual bool CanParse(const std::string& line) const = 0;
};

class RegexCurrencyRateParser : public ICurrencyRateParser {
public:
  CurrencyRate Parse(const std::string& line) const override;
  bool CanParse(const std::string& line) const override;

private:
  static const std::regex kPattern;
};

class CurrencyRateParserFactory {
public:
  static std::unique_ptr<ICurrencyRateParser> CreateDefaultParser();
};

#endif  // CURRENCY_RATE_PARSER_H_