// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#include "currency_rate_parser.h"

#include <regex>

using std::invalid_argument;
using std::make_unique;
using std::regex;
using std::regex_match;
using std::smatch;
using std::stod;
using std::string;
using std::unique_ptr;

const regex RegexCurrencyRateParser::kPattern(
    "^\\s*(\"([^\"]*)\"|([^ \"]+))\\s+(\"([^\"]*)\"|([^ \"]+))\\s+([\\d.]+)\\s+(\\d{4}\\.\\d{2}\\.\\d{2})\\s*$");

CurrencyRate RegexCurrencyRateParser::Parse(const string& line) const {
  smatch matches;

  if (!regex_match(line, matches, kPattern)) {
    throw InvalidFormatException(
        "Line does not match expected format: " + line);
  }

  try {
    string currency1 = matches[2].matched ?
        matches[2].str() : matches[3].str();

    string currency2 = matches[5].matched ?
        matches[5].str() : matches[6].str();

    double rate = stod(matches[7].str());
    string date = matches[8].str();

    return CurrencyRate(currency1, currency2, rate, date);

  } catch (const std::exception& e) {
    throw InvalidFormatException(
        "Error parsing line: " + string(e.what()));
  }
}

bool RegexCurrencyRateParser::CanParse(const string& line) const {
  return regex_match(line, kPattern);
}

unique_ptr<ICurrencyRateParser>
CurrencyRateParserFactory::CreateDefaultParser() {
  return make_unique<RegexCurrencyRateParser>();
}