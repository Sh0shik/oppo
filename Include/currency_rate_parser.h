// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#ifndef CURRENCY_RATE_PARSER_H_
#define CURRENCY_RATE_PARSER_H_

#include <memory>
#include <regex>

#include "currency_rate.h"

class ICurrencyRateParser {
public:
  virtual ~ICurrencyRateParser() = default;
  virtual CurrencyRate parse(const std::string& line) const = 0;
  virtual bool can_parse(const std::string& line) const = 0;
};

class RegexCurrencyRateParser : public ICurrencyRateParser {
public:
  CurrencyRate parse(const std::string& line) const override;
  bool can_parse(const std::string& line) const override;

private:
  static const std::regex kPattern;
};

class CurrencyRateParserFactory {
public:
  static std::unique_ptr<ICurrencyRateParser> create_default_parser();
};

#endif  // CURRENCY_RATE_PARSER_H_