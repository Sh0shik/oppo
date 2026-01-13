// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#include "currency_rate_repository.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::function;
using std::getline;
using std::ifstream;
using std::ios;
using std::make_unique;
using std::move;
using std::ofstream;
using std::runtime_error;
using std::sort;
using std::string;
using std::unique_ptr;
using std::vector;

MemoryCurrencyRateRepository::MemoryCurrencyRateRepository(
    unique_ptr<ICurrencyRateParser> parser)
    : parser_(move(parser)) {}

void MemoryCurrencyRateRepository::Add(const CurrencyRate& rate) {
  rates_.push_back(rate);
}

vector<CurrencyRate> MemoryCurrencyRateRepository::GetAll() const {
  return rates_;
}

size_t MemoryCurrencyRateRepository::Count() const {
  return rates_.size();
}

void MemoryCurrencyRateRepository::Clear() {
  rates_.clear();
}

void MemoryCurrencyRateRepository::SortByDate() {
  Sort([](const CurrencyRate& a, const CurrencyRate& b) {
    return a < b;
  });
}

void MemoryCurrencyRateRepository::SortByCurrency() {
  Sort([](const CurrencyRate& a, const CurrencyRate& b) {
    if (a.currency1() != b.currency1()) {
      return a.currency1() < b.currency1();
    }
    return a.currency2() < b.currency2();
  });
}

void MemoryCurrencyRateRepository::Sort(
    const function<bool(const CurrencyRate&, const CurrencyRate&)>& comparator) {
  std::sort(rates_.begin(), rates_.end(), comparator);
}

void MemoryCurrencyRateRepository::AddFromFile(const string& filename) {
  ifstream file(filename);

  if (!file.is_open()) {
    throw runtime_error("Failed to open file: " + filename);
  }

  string line;
  int line_number = 0;
  int successfully_parsed = 0;

  while (getline(file, line)) {
    line_number++;

    if (line.empty() || std::all_of(line.begin(), line.end(),
        [](unsigned char c) { return std::isspace(c); })) {
      continue;
    }

    try {
      if (parser_->CanParse(line)) {
        CurrencyRate rate = parser_->Parse(line);
        rates_.push_back(rate);
        successfully_parsed++;
      } else {
        cerr << "Warning: line " << line_number
             << " has invalid format and will be skipped: "
             << line << endl;
      }
    } catch (const CurrencyRateException& e) {
      cerr << "Error parsing line " << line_number
           << ": " << e.what() << endl;
    } catch (const std::exception& e) {
      cerr << "Unexpected error parsing line " << line_number
           << ": " << e.what() << endl;
    }
  }

  file.close();

  if (successfully_parsed == 0 && line_number > 0) {
    cerr << "Warning: no lines were successfully parsed!" << endl;
  }
}

void MemoryCurrencyRateRepository::SaveToFile(const string& filename) const {
  ofstream file(filename);

  if (!file.is_open()) {
    throw runtime_error("Failed to open file for writing: " + filename);
  }

  for (const auto& rate : rates_) {
    file << rate.ToFileString() << endl;
  }

  file.close();
}

void MemoryCurrencyRateRepository::AppendToFile(const string& filename,
                                                const CurrencyRate& rate) const {
  ofstream file(filename, ios::app);

  if (!file.is_open()) {
    throw runtime_error("Failed to open file for appending: " + filename);
  }

  file << rate.ToFileString() << endl;
  file.close();
}