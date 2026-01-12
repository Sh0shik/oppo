// Copyright 2024 Maslakov Saveliy KI24-07B. All rights reserved.

#include <clocale>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "currency_rate.h"
#include "currency_rate_parser.h"
#include "currency_rate_repository.h"
#include "currency_rate_validator.h"

using std::cin;
using std::cout;
using std::endl;
using std::function;
using std::getline;
using std::make_shared;
using std::make_unique;
using std::map;
using std::move;
using std::ostringstream;
using std::setprecision;
using std::shared_ptr;
using std::stod;
using std::stoi;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;

namespace {

bool ShowAllRates(shared_ptr<ICurrencyRateRepository> repository) {
  cout << "\n=== All Currency Rates ===" << endl;
  auto rates = repository->GetAll();

  if (rates.empty()) {
    cout << "No data to display." << endl;
  } else {
    cout << "Total records: " << rates.size() << endl;
    cout << "----------------------------------------" << endl;
    for (const auto& rate : rates) {
      cout << "Currency 1: " << rate.currency1() << endl;
      cout << "Currency 2: " << rate.currency2() << endl;
      cout << std::fixed << setprecision(4)
           << "Rate: " << rate.rate() << endl;
      cout << "Date: " << rate.date() << endl;
      cout << "----------------------------------------" << endl;
    }
  }
  return true;
}

bool SortByDateMenu(shared_ptr<ICurrencyRateRepository> repository) {
  auto rates = repository->GetAll();

  if (rates.empty()) {
    cout << "No data to sort." << endl;
    return true;
  }

  repository->SortByDate();
  cout << "\n=== Data sorted by date ===" << endl;

  auto sorted_rates = repository->GetAll();
  for (const auto& rate : sorted_rates) {
    cout << "Date: " << rate.date()
         << " | " << rate.currency1() << "/" << rate.currency2()
         << " = " << std::fixed << setprecision(4) << rate.rate() << endl;
  }
  return true;
}

bool SortByCurrencyMenu(shared_ptr<ICurrencyRateRepository> repository) {
  auto rates = repository->GetAll();

  if (rates.empty()) {
    cout << "No data to sort." << endl;
    return true;
  }

  repository->SortByCurrency();
  cout << "\n=== Data sorted by currency ===" << endl;

  auto sorted_rates = repository->GetAll();
  for (const auto& rate : sorted_rates) {
    cout << rate.currency1() << "/" << rate.currency2()
         << " | " << std::fixed << setprecision(4) << rate.rate()
         << " | " << rate.date() << endl;
  }
  return true;
}

bool FilterByCurrencyMenu(shared_ptr<ICurrencyRateRepository> repository) {
  auto rates = repository->GetAll();

  if (rates.empty()) {
    cout << "No data to filter." << endl;
    return true;
  }

  string currency_filter;
  cout << "Enter currency to filter by (e.g., USD or EUR): ";
  getline(cin, currency_filter);

  if (currency_filter.empty()) {
    cout << "Error: currency name cannot be empty!" << endl;
    return true;
  }

  vector<CurrencyRate> filtered;
  for (const auto& rate : rates) {
    if (rate.currency1() == currency_filter ||
        rate.currency2() == currency_filter) {
      filtered.push_back(rate);
    }
  }

  cout << "\n=== Data for currency '" << currency_filter << "' ===" << endl;
  cout << "Found records: " << filtered.size() << endl;

  if (filtered.empty()) {
    cout << "No records found for specified currency." << endl;
  } else {
    for (const auto& data : filtered) {
      cout << data.currency1() << "/" << data.currency2()
           << " | " << std::fixed << setprecision(4) << data.rate()
           << " | " << data.date() << endl;
    }
  }
  return true;
}

bool FilterByDateMenu(shared_ptr<ICurrencyRateRepository> repository) {
  auto rates = repository->GetAll();

  if (rates.empty()) {
    cout << "No data to filter." << endl;
    return true;
  }

  string date_filter;
  cout << "Enter date to filter by (YYYY.MM.DD): ";
  getline(cin, date_filter);

  try {
    CurrencyRateValidator::ValidateDate(date_filter);
  } catch (const CurrencyRateException& e) {
    cout << "Error: " << e.what() << endl;
    return true;
  }

  vector<CurrencyRate> filtered;
  for (const auto& rate : rates) {
    if (rate.date() == date_filter) {
      filtered.push_back(rate);
    }
  }

  cout << "\n=== Data for date '" << date_filter << "' ===" << endl;
  cout << "Found records: " << filtered.size() << endl;

  if (filtered.empty()) {
    cout << "No records found for specified date." << endl;
  } else {
    for (const auto& data : filtered) {
      cout << data.currency1() << "/" << data.currency2()
           << " = " << std::fixed << setprecision(4) << data.rate() << endl;
    }
  }
  return true;
}

string ReadString(const string& prompt, bool required = true) {
  string value;
  while (true) {
    cout << prompt;
    getline(cin, value);

    size_t start = value.find_first_not_of(" \t");
    size_t end = value.find_last_not_of(" \t");

    if (start == string::npos) {
      value = "";
    } else {
      value = value.substr(start, end - start + 1);
    }

    if (required && value.empty()) {
      cout << "Error: field cannot be empty!" << endl;
    } else {
      break;
    }
  }
  return value;
}

double ReadRate() {
  double value;
  while (true) {
    string input = ReadString("Enter exchange rate: ", true);

    try {
      value = stod(input);

      try {
        CurrencyRateValidator::ValidateRate(value);
        break;
      } catch (const CurrencyRateException& e) {
        cout << "Error: " << e.what() << endl;
      }
    } catch (const std::invalid_argument&) {
      cout << "Error: enter a valid numeric value!" << endl;
    } catch (const std::out_of_range&) {
      cout << "Error: number out of valid range!" << endl;
    }
  }
  return value;
}

string ReadDate() {
  string date;
  while (true) {
    date = ReadString("Enter date (YYYY.MM.DD): ", true);

    try {
      CurrencyRateValidator::ValidateDate(date);
      break;
    } catch (const CurrencyRateException& e) {
      cout << "Error: " << e.what() << endl;
    }
  }
  return date;
}

bool AddManualData(shared_ptr<ICurrencyRateRepository> repository,
                   const string& filename) {
  cout << "\n=== Manual Data Entry ===" << endl;

  try {
    string currency1 = ReadString("Enter first currency: ", true);
    CurrencyRateValidator::ValidateCurrencyName(currency1);

    string currency2 = ReadString("Enter second currency: ", true);
    CurrencyRateValidator::ValidateCurrencyName(currency2);

    double rate = ReadRate();
    string date = ReadDate();

    CurrencyRate new_data(currency1, currency2, rate, date);
    repository->Add(new_data);

    auto memory_repo = std::dynamic_pointer_cast<MemoryCurrencyRateRepository>(
        repository);
    if (memory_repo) {
      memory_repo->AppendToFile(filename, new_data);
    }

    cout << "\nData successfully added!" << endl;
    cout << "Currency 1: " << new_data.currency1() << endl;
    cout << "Currency 2: " << new_data.currency2() << endl;
    cout << std::fixed << setprecision(4)
         << "Rate: " << new_data.rate() << endl;
    cout << "Date: " << new_data.date() << endl;

  } catch (const CurrencyRateException& e) {
    cout << "Error adding data: " << e.what() << endl;
  } catch (const std::exception& e) {
    cout << "Unexpected error: " << e.what() << endl;
  }

  return true;
}

bool SaveToFileMenu(shared_ptr<ICurrencyRateRepository> repository) {
  string filename;
  cout << "Enter filename to save: ";
  getline(cin, filename);

  if (filename.empty()) {
    cout << "Error: filename cannot be empty!" << endl;
    return true;
  }

  try {
    auto memory_repo = std::dynamic_pointer_cast<MemoryCurrencyRateRepository>(
        repository);
    if (memory_repo) {
      memory_repo->SaveToFile(filename);
      cout << "Data successfully saved to file: " << filename << endl;
    } else {
      cout << "Error: operation not supported" << endl;
    }
  } catch (const std::exception& e) {
    cout << "Error saving file: " << e.what() << endl;
  }

  return true;
}

bool ExitProgram(shared_ptr<ICurrencyRateRepository> repository) {
  cout << "Exiting program." << endl;
  return false;
}

}  // namespace

int main() {
  setlocale(LC_ALL, "en_US.UTF-8");
  cout << "=== Currency Rate Manager ===" << endl;

  string filename;
  cout << "Enter data filename: ";
  getline(cin, filename);

  if (filename.empty()) {
    cout << "Filename not specified. "
         << "Using default file: rates.txt" << endl;
    filename = "rates.txt";
  }

  auto parser = make_unique<RegexCurrencyRateParser>();
  auto repository = make_shared<MemoryCurrencyRateRepository>(move(parser));

  try {
    repository->AddFromFile(filename);
    cout << "Successfully loaded records from file: "
         << repository->Count() << endl;
  } catch (const std::exception& e) {
    cout << "Warning: " << e.what() << endl;
    cout << "Starting with empty database." << endl;
  }

  int choice;
  bool should_continue = true;

  const map<int, function<bool()>> menu_functions = {
    {1, [&]() { return ShowAllRates(repository); }},
    {2, [&]() { return SortByDateMenu(repository); }},
    {3, [&]() { return SortByCurrencyMenu(repository); }},
    {4, [&]() { return FilterByCurrencyMenu(repository); }},
    {5, [&]() { return FilterByDateMenu(repository); }},
    {6, [&]() { return AddManualData(repository, filename); }},
    {7, [&]() { return SaveToFileMenu(repository); }},
    {8, [&]() { return ExitProgram(repository); }}
  };

  do {
    cout << "\n=== MENU ===" << endl;
    cout << "1. Show all rates" << endl;
    cout << "2. Sort by date" << endl;
    cout << "3. Sort by currency" << endl;
    cout << "4. Filter by currency" << endl;
    cout << "5. Filter by date" << endl;
    cout << "6. Add rate manually" << endl;
    cout << "7. Save to file" << endl;
    cout << "8. Exit" << endl;
    cout << "Choose action: ";

    string input;
    getline(cin, input);

    if (input.empty()) {
      cout << "Error: enter a number from 1 to 8!" << endl;
      continue;
    }

    try {
      choice = stoi(input);

      if (menu_functions.find(choice) != menu_functions.end()) {
        should_continue = menu_functions.at(choice)();
      } else {
        cout << "Error: invalid menu item! Choose from 1 to 8." << endl;
      }
    } catch (const std::invalid_argument&) {
      cout << "Error: enter a valid number!" << endl;
    } catch (const std::exception& e) {
      cout << "Unexpected error: " << e.what() << endl;
    }

  } while (should_continue);

  return 0;
}