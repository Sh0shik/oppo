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
  virtual void add(const CurrencyRate& rate) = 0;
  virtual std::vector<CurrencyRate> get_all() const = 0;
  virtual size_t count() const = 0;
  virtual void clear() = 0;
  virtual void sort_by_date() = 0;
  virtual void sort_by_currency() = 0;
};

class MemoryCurrencyRateRepository : public ICurrencyRateRepository {
public:
  explicit MemoryCurrencyRateRepository(
      std::unique_ptr<ICurrencyRateParser> parser);

  void add(const CurrencyRate& rate) override;
  std::vector<CurrencyRate> get_all() const override;
  size_t count() const override;
  void clear() override;
  void sort_by_date() override;
  void sort_by_currency() override;

  void add_from_file(const std::string& filename);
  void save_to_file(const std::string& filename) const;
  void append_to_file(const std::string& filename,
                      const CurrencyRate& rate) const;

private:
  std::vector<CurrencyRate> rates_;
  std::unique_ptr<ICurrencyRateParser> parser_;

  void sort(const std::function<bool(const CurrencyRate&,
                                     const CurrencyRate&)>& comparator);
};

#endif  // CURRENCY_RATE_REPOSITORY_H_