// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#ifndef CURRENCY_RATE_REPOSITORY_H_
#define CURRENCY_RATE_REPOSITORY_H_

#include <functional>
#include <memory>
#include <vector>

#include "currency_rate.h"
#include "currency_rate_parser.h"

class ICurrencyRateRepository {
public:
  virtual ~ICurrencyRateRepository() = default;
  virtual void Add(const CurrencyRate& rate) = 0;
  virtual std::vector<CurrencyRate> GetAll() const = 0;
  virtual size_t Count() const = 0;
  virtual void Clear() = 0;
  virtual void SortByDate() = 0;
  virtual void SortByCurrency() = 0;
};

class MemoryCurrencyRateRepository : public ICurrencyRateRepository {
public:
  explicit MemoryCurrencyRateRepository(
      std::unique_ptr<ICurrencyRateParser> parser);

  void Add(const CurrencyRate& rate) override;
  std::vector<CurrencyRate> GetAll() const override;
  size_t Count() const override;
  void Clear() override;
  void SortByDate() override;
  void SortByCurrency() override;

  void AddFromFile(const std::string& filename);
  void SaveToFile(const std::string& filename) const;
  void AppendToFile(const std::string& filename,
                    const CurrencyRate& rate) const;

private:
  std::vector<CurrencyRate> rates_;
  std::unique_ptr<ICurrencyRateParser> parser_;

  void Sort(const std::function<bool(const CurrencyRate&,
                                     const CurrencyRate&)>& comparator);
};

#endif  // CURRENCY_RATE_REPOSITORY_H_