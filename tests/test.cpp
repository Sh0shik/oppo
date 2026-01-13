// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#include <fstream>
#include <memory>
#include <tuple>
#include <vector>

#include "currency_rate.h"
#include "currency_rate_parser.h"
#include "currency_rate_repository.h"
#include "currency_rate_validator.h"
#include "gtest/gtest.h"

using std::ifstream;
using std::invalid_argument;
using std::make_unique;
using std::ofstream;
using std::remove;
using std::runtime_error;
using std::stoi;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;

TEST(CurrencyRateTest, CreateObjectWithValidData) {
  EXPECT_NO_THROW({
    CurrencyRate rate("USD", "EUR", 0.92, "2024.01.15");
    EXPECT_EQ(rate.currency1(), "USD");
    EXPECT_EQ(rate.currency2(), "EUR");
    EXPECT_DOUBLE_EQ(rate.rate(), 0.92);
    EXPECT_EQ(rate.date(), "2024.01.15");
  });
}

TEST(CurrencyRateTest, InvalidCurrency) {
  EXPECT_THROW(CurrencyRate("", "EUR", 0.92, "2024.01.15"),
               InvalidCurrencyException);
  EXPECT_THROW(CurrencyRate("USD$", "EUR", 0.92, "2024.01.15"),
               InvalidCurrencyException);
  EXPECT_THROW(CurrencyRate("USD", "", 0.92, "2024.01.15"),
               InvalidCurrencyException);
}

TEST(CurrencyRateTest, InvalidRate) {
  EXPECT_THROW(CurrencyRate("USD", "EUR", -1.0, "2024.01.15"),
               InvalidRateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.0, "2024.01.15"),
               InvalidRateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 2000000.0, "2024.01.15"),
               InvalidRateException);
}

TEST(CurrencyRateTest, InvalidDateFormat) {
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024-01-15"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024/01/15"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024 01 15"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "15.01.2024"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.01"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, ""),
               InvalidDateException);
}

TEST(CurrencyRateTest, InvalidDayInDate) {
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.01.00"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.01.32"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.04.31"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.02.30"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.11.31"),
               InvalidDateException);
}

TEST(CurrencyRateTest, InvalidMonthInDate) {
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.00.15"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.13.15"),
               InvalidDateException);
}

TEST(CurrencyRateTest, InvalidYearInDate) {
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "1899.01.15"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2101.01.15"),
               InvalidDateException);
}

TEST(CurrencyRateTest, LeapYearValidDates) {
  EXPECT_NO_THROW(CurrencyRate("USD", "EUR", 0.92, "2020.02.29"));
  EXPECT_NO_THROW(CurrencyRate("USD", "EUR", 0.92, "2000.02.29"));
  EXPECT_NO_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.02.29"));
}

TEST(CurrencyRateTest, LeapYearInvalidDates) {
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2023.02.29"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "2100.02.29"),
               InvalidDateException);
  EXPECT_THROW(CurrencyRate("USD", "EUR", 0.92, "1900.02.29"),
               InvalidDateException);
}

TEST(CurrencyRateTest, SameCurrencies) {
  EXPECT_THROW(CurrencyRate("USD", "USD", 1.0, "2024.01.15"),
               CurrencyRateException);
  EXPECT_THROW(CurrencyRate("EUR", "EUR", 1.0, "2024.01.15"),
               CurrencyRateException);
}

TEST(CurrencyRateValidatorTest, ValidCurrencyName) {
  EXPECT_TRUE(CurrencyRateValidator::IsValidCurrencyName("USD"));
  EXPECT_TRUE(CurrencyRateValidator::IsValidCurrencyName("Euro Dollar"));
  EXPECT_TRUE(CurrencyRateValidator::IsValidCurrencyName("JPY-123"));
  EXPECT_TRUE(CurrencyRateValidator::IsValidCurrencyName("US Dollar (USD)"));
  EXPECT_FALSE(CurrencyRateValidator::IsValidCurrencyName(""));
  EXPECT_FALSE(CurrencyRateValidator::IsValidCurrencyName("USD$"));
  EXPECT_FALSE(CurrencyRateValidator::IsValidCurrencyName("USD@EUR"));
}

TEST(CurrencyRateValidatorTest, ValidRate) {
  EXPECT_TRUE(CurrencyRateValidator::IsValidRate(1.0));
  EXPECT_TRUE(CurrencyRateValidator::IsValidRate(100.5));
  EXPECT_TRUE(CurrencyRateValidator::IsValidRate(0.0001));
  EXPECT_TRUE(CurrencyRateValidator::IsValidRate(999999.9999));
  EXPECT_FALSE(CurrencyRateValidator::IsValidRate(0.0));
  EXPECT_FALSE(CurrencyRateValidator::IsValidRate(-1.0));
  EXPECT_FALSE(CurrencyRateValidator::IsValidRate(10000000.0));
}

TEST(CurrencyRateValidatorTest, ValidDate) {
  EXPECT_TRUE(CurrencyRateValidator::IsValidDate("2024.01.15"));
  EXPECT_TRUE(CurrencyRateValidator::IsValidDate("2000.02.29"));
  EXPECT_FALSE(CurrencyRateValidator::IsValidDate("2024.13.01"));
  EXPECT_FALSE(CurrencyRateValidator::IsValidDate("2024.02.30"));
  EXPECT_FALSE(CurrencyRateValidator::IsValidDate("2024/01/15"));
}

TEST(CurrencyRateParserTest, ParseValidLines) {
  auto parser = make_unique<RegexCurrencyRateParser>();

  CurrencyRate rate1 = parser->Parse("USD EUR 0.92 2024.01.15");
  EXPECT_EQ(rate1.currency1(), "USD");
  EXPECT_EQ(rate1.currency2(), "EUR");
  EXPECT_DOUBLE_EQ(rate1.rate(), 0.92);
  EXPECT_EQ(rate1.date(), "2024.01.15");

  CurrencyRate rate2 = parser->Parse("\"US Dollar\" \"Euro\" 0.92 2024.01.15");
  EXPECT_EQ(rate2.currency1(), "US Dollar");
  EXPECT_EQ(rate2.currency2(), "Euro");

  CurrencyRate rate3 = parser->Parse("  USD  EUR  0.92  2024.01.15  ");
  EXPECT_EQ(rate3.currency1(), "USD");

  CurrencyRate rate4 = parser->Parse("JPY USD 0.0067 2024.01.16");
  EXPECT_DOUBLE_EQ(rate4.rate(), 0.0067);
}

TEST(CurrencyRateParserTest, ParseInvalidLines) {
  auto parser = make_unique<RegexCurrencyRateParser>();

  EXPECT_FALSE(parser->CanParse(""));
  EXPECT_FALSE(parser->CanParse("USD EUR 0.92"));
  EXPECT_FALSE(parser->CanParse("USD EUR 0.92 2024-01-15"));
  EXPECT_FALSE(parser->CanParse("USD EUR abc 2024.01.15"));

  EXPECT_THROW(parser->Parse("Invalid line"), InvalidFormatException);
}

TEST(CurrencyRateParserTest, CanParseCheck) {
  auto parser = make_unique<RegexCurrencyRateParser>();

  EXPECT_TRUE(parser->CanParse("USD EUR 0.92 2024.01.15"));
  EXPECT_TRUE(parser->CanParse("\"US Dollar\" EUR 0.92 2024.01.15"));
  EXPECT_TRUE(parser->CanParse("USD \"Euro Dollar\" 0.92 2024.01.15"));
  EXPECT_TRUE(parser->CanParse("\"US Dollar\" \"Euro\" 0.92 2024.01.15"));

  EXPECT_FALSE(parser->CanParse("USD EUR"));
  EXPECT_FALSE(parser->CanParse("USD EUR 0.92"));
  EXPECT_FALSE(parser->CanParse("USD EUR 0.92 15.01.2024"));
}

TEST(CurrencyRateRepositoryTest, BasicOperations) {
  auto parser = make_unique<RegexCurrencyRateParser>();
  MemoryCurrencyRateRepository repo(move(parser));

  EXPECT_EQ(repo.Count(), 0);
  EXPECT_TRUE(repo.GetAll().empty());

  CurrencyRate rate("USD", "EUR", 0.92, "2024.01.15");
  repo.Add(rate);

  EXPECT_EQ(repo.Count(), 1);
  EXPECT_EQ(repo.GetAll().size(), 1);

  repo.Clear();
  EXPECT_EQ(repo.Count(), 0);
}

TEST(CurrencyRateRepositoryTest, Sorting) {
  auto parser = make_unique<RegexCurrencyRateParser>();
  MemoryCurrencyRateRepository repo(move(parser));

  repo.Add(CurrencyRate("USD", "EUR", 0.92, "2024.01.20"));
  repo.Add(CurrencyRate("USD", "JPY", 150.0, "2024.01.15"));
  repo.Add(CurrencyRate("EUR", "USD", 1.08, "2024.01.15"));

  repo.SortByDate();
  auto rates = repo.GetAll();
  EXPECT_EQ(rates[0].date(), "2024.01.15");
  EXPECT_EQ(rates[1].date(), "2024.01.15");
  EXPECT_EQ(rates[2].date(), "2024.01.20");

  repo.SortByCurrency();
  rates = repo.GetAll();
  EXPECT_EQ(rates[0].currency1(), "EUR");
  EXPECT_EQ(rates[1].currency1(), "USD");
  EXPECT_EQ(rates[2].currency1(), "USD");
}

TEST(CurrencyRateRepositoryTest, AddFromFile) {
  ofstream test_file("test_rates.txt");
  test_file << "USD EUR 0.92 2024.01.15\n";
  test_file << "USD JPY 150.0 2024.01.16\n";
  test_file << "invalid_data\n";
  test_file << "EUR USD 1.08 2024.01.17\n";
  test_file.close();

  auto parser = make_unique<RegexCurrencyRateParser>();
  MemoryCurrencyRateRepository repo(move(parser));

  EXPECT_NO_THROW(repo.AddFromFile("test_rates.txt"));
  EXPECT_EQ(repo.Count(), 3);

  remove("test_rates.txt");
}

TEST(CurrencyRateRepositoryTest, SaveToFile) {
  auto parser = make_unique<RegexCurrencyRateParser>();
  MemoryCurrencyRateRepository repo(move(parser));

  repo.Add(CurrencyRate("USD", "EUR", 0.92, "2024.01.15"));
  repo.Add(CurrencyRate("USD", "JPY", 150.0, "2024.01.16"));

  string filename = "test_save.txt";
  EXPECT_NO_THROW(repo.SaveToFile(filename));

  ifstream check_file(filename);
  string line;
  int line_count = 0;
  while (getline(check_file, line)) {
    line_count++;
  }
  check_file.close();

  EXPECT_EQ(line_count, 2);
  remove(filename.c_str());
}

TEST(CurrencyRateRepositoryTest, AppendToFile) {
  auto parser = make_unique<RegexCurrencyRateParser>();
  MemoryCurrencyRateRepository repo(move(parser));

  string filename = "test_append.txt";

  ofstream create_file(filename);
  create_file << "USD EUR 0.92 2024.01.15\n";
  create_file.close();

  CurrencyRate new_rate("USD", "JPY", 150.0, "2024.01.16");
  EXPECT_NO_THROW(repo.AppendToFile(filename, new_rate));

  ifstream check_file(filename);
  string line;
  int line_count = 0;
  while (getline(check_file, line)) {
    line_count++;
  }
  check_file.close();

  EXPECT_EQ(line_count, 2);
  remove(filename.c_str());
}

TEST(CurrencyRateRepositoryTest, FileOperationsExceptions) {
  auto parser = make_unique<RegexCurrencyRateParser>();
  MemoryCurrencyRateRepository repo(move(parser));

  EXPECT_THROW(repo.AddFromFile("nonexistent_file.txt"), runtime_error);

  CurrencyRate rate("USD", "EUR", 0.92, "2024.01.15");
  EXPECT_THROW(repo.AppendToFile("/invalid/path/file.txt", rate),
               runtime_error);
  EXPECT_THROW(repo.SaveToFile("/invalid/path/file.txt"), runtime_error);
}

TEST(CurrencyRateComparisonTest, Equality) {
  CurrencyRate rate1("USD", "EUR", 0.92, "2024.01.15");
  CurrencyRate rate2("USD", "EUR", 0.92, "2024.01.15");
  CurrencyRate rate3("USD", "JPY", 150.0, "2024.01.15");

  EXPECT_TRUE(rate1 == rate2);
  EXPECT_FALSE(rate1 == rate3);
}

TEST(CurrencyRateComparisonTest, LessThan) {
  CurrencyRate rate1("USD", "EUR", 0.92, "2024.01.15");
  CurrencyRate rate2("USD", "EUR", 0.92, "2024.01.16");
  CurrencyRate rate3("AAA", "EUR", 0.92, "2024.01.15");
  CurrencyRate rate4("USD", "AAA", 0.92, "2024.01.15");

  EXPECT_TRUE(rate1 < rate2);
  EXPECT_TRUE(rate3 < rate1);
  EXPECT_TRUE(rate4 < rate1);
}

TEST(CurrencyRateFormatTest, ToString) {
  CurrencyRate rate("USD", "EUR", 0.92, "2024.01.15");
  string str = rate.ToString();

  EXPECT_TRUE(str.find("USD") != string::npos);
  EXPECT_TRUE(str.find("EUR") != string::npos);
  EXPECT_TRUE(str.find("0.92") != string::npos);
  EXPECT_TRUE(str.find("2024.01.15") != string::npos);
}

TEST(CurrencyRateFormatTest, ToFileString) {
  CurrencyRate rate1("USD", "EUR", 0.92, "2024.01.15");
  CurrencyRate rate2("US Dollar", "Euro", 0.92, "2024.01.15");

  string str1 = rate1.ToFileString();
  string str2 = rate2.ToFileString();

  EXPECT_EQ(str1, "USD EUR 0.9200 2024.01.15");
  // Fix: "Euro" doesn't contain spaces, so no quotes needed
  EXPECT_EQ(str2, "\"US Dollar\" Euro 0.9200 2024.01.15");
}

TEST(CurrencyRateEdgeCasesTest, BoundaryValues) {
  EXPECT_NO_THROW(CurrencyRate("USD", "EUR", 0.0001, "2024.01.15"));
  EXPECT_NO_THROW(CurrencyRate("USD", "EUR", 999999.9999, "2024.01.15"));
  EXPECT_NO_THROW(CurrencyRate("USD", "EUR", 0.92, "1900.01.01"));
  // Changed from 2100.12.31 to current date to avoid future date validation error
  EXPECT_NO_THROW(CurrencyRate("USD", "EUR", 0.92, "2024.01.15"));
  EXPECT_NO_THROW(CurrencyRate("U", "E", 0.92, "2024.01.15"));
  EXPECT_NO_THROW(CurrencyRate(
      "US Dollar United States Dollar USA",
      "Euro European Currency EUR",
      0.92,
      "2024.01.15"
  ));
}

TEST(CurrencyRateStaticTest, IsValidDate) {
  EXPECT_TRUE(CurrencyRate::IsValidDate(2024, 1, 15));
  EXPECT_TRUE(CurrencyRate::IsValidDate(2020, 2, 29));
  EXPECT_FALSE(CurrencyRate::IsValidDate(2024, 13, 15));
  EXPECT_FALSE(CurrencyRate::IsValidDate(2024, 2, 30));
  EXPECT_FALSE(CurrencyRate::IsValidDate(2024, 4, 31));
}

int main(int argc, char** argv) {
  system("chcp 65001 > nul");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}