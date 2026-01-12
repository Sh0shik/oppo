// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#include "currency_rate_repository.h"

#include <algorithm>
#include <fstream>
#include <iostream>

using std::all_of;
using std::cerr;
using std::cout;
using std::endl;
using std::function;
using std::getline;
using std::ifstream;
using std::ios;
using std::isspace;
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

void MemoryCurrencyRateRepository::add(const CurrencyRate& rate) {
  rates_.push_back(rate);
}

vector<CurrencyRate> MemoryCurrencyRateRepository::get_all() const {
  return rates_;
}

size_t MemoryCurrencyRateRepository::count() const {
  return rates_.size();
}

void MemoryCurrencyRateRepository::clear() {
  rates_.clear();
}

void MemoryCurrencyRateRepository::sort_by_date() {
  sort([](const CurrencyRate& a, const CurrencyRate& b) {
    return a < b;
  });
}

void MemoryCurrencyRateRepository::sort_by_currency() {
  sort([](const CurrencyRate& a, const CurrencyRate& b) {
    if (a.currency1() != b.currency1()) {
      return a.currency1() < b.currency1();
    }
    return a.currency2() < b.currency2();
  });
}

void MemoryCurrencyRateRepository::sort(
    const function<bool(const CurrencyRate&, const CurrencyRate&)>& comparator) {
  std::sort(rates_.begin(), rates_.end(), comparator);
}

void MemoryCurrencyRateRepository::add_from_file(const string& filename) {
  ifstream file(filename);

  if (!file.is_open()) {
    throw runtime_error("Не удалось открыть файл: " + filename);
  }

  string line;
  int line_number = 0;
  int successfully_parsed = 0;

  while (getline(file, line)) {
    line_number++;

    if (line.empty() || all_of(line.begin(), line.end(), ::isspace)) {
      continue;
    }

    try {
      if (parser_->can_parse(line)) {
        CurrencyRate rate = parser_->parse(line);
        rates_.push_back(rate);
        successfully_parsed++;
      } else {
        cerr << "Предупреждение: строка " << line_number
             << " имеет неверный формат и будет пропущена: "
             << line << endl;
      }
    } catch (const CurrencyRateException& e) {
      cerr << "Ошибка при разборе строки " << line_number
           << ": " << e.what() << endl;
    } catch (const std::exception& e) {
      cerr << "Неожиданная ошибка при разборе строки " << line_number
           << ": " << e.what() << endl;
    }
  }

  file.close();

  if (successfully_parsed == 0 && line_number > 0) {
    cerr << "Внимание: ни одна строка не была успешно разобрана!" << endl;
  }
}

void MemoryCurrencyRateRepository::save_to_file(const string& filename) const {
  ofstream file(filename);

  if (!file.is_open()) {
    throw runtime_error("Не удалось открыть файл для записи: " + filename);
  }

  for (const auto& rate : rates_) {
    file << rate.ToFileString() << endl;
  }

  file.close();
}

void MemoryCurrencyRateRepository::append_to_file(const string& filename,
                                                  const CurrencyRate& rate) const {
  ofstream file(filename, ios::app);

  if (!file.is_open()) {
    throw runtime_error("Не удалось открыть файл для добавления: " + filename);
  }

  file << rate.ToFileString() << endl;
  file.close();
}