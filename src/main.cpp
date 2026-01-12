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
  cout << "\n=== Все курсы валют ===" << endl;
  auto rates = repository->get_all();

  if (rates.empty()) {
    cout << "Нет данных для отображения." << endl;
  } else {
    cout << "Всего записей: " << rates.size() << endl;
    cout << "----------------------------------------" << endl;
    for (const auto& rate : rates) {
      cout << "Валюта 1: " << rate.currency1() << endl;
      cout << "Валюта 2: " << rate.currency2() << endl;
      cout << std::fixed << setprecision(4)
           << "Курс: " << rate.rate() << endl;
      cout << "Дата: " << rate.date() << endl;
      cout << "----------------------------------------" << endl;
    }
  }
  return true;
}

bool SortByDateMenu(shared_ptr<ICurrencyRateRepository> repository) {
  auto rates = repository->get_all();

  if (rates.empty()) {
    cout << "Нет данных для сортировки." << endl;
    return true;
  }

  repository->sort_by_date();
  cout << "\n=== Данные отсортированы по дате ===" << endl;

  auto sorted_rates = repository->get_all();
  for (const auto& rate : sorted_rates) {
    cout << "Дата: " << rate.date()
         << " | " << rate.currency1() << "/" << rate.currency2()
         << " = " << std::fixed << setprecision(4) << rate.rate() << endl;
  }
  return true;
}

bool SortByCurrencyMenu(shared_ptr<ICurrencyRateRepository> repository) {
  auto rates = repository->get_all();

  if (rates.empty()) {
    cout << "Нет данных для сортировки." << endl;
    return true;
  }

  repository->sort_by_currency();
  cout << "\n=== Данные отсортированы по валютам ===" << endl;

  auto sorted_rates = repository->get_all();
  for (const auto& rate : sorted_rates) {
    cout << rate.currency1() << "/" << rate.currency2()
         << " | " << std::fixed << setprecision(4) << rate.rate()
         << " | " << rate.date() << endl;
  }
  return true;
}

bool FilterByCurrencyMenu(shared_ptr<ICurrencyRateRepository> repository) {
  auto rates = repository->get_all();

  if (rates.empty()) {
    cout << "Нет данных для фильтрации." << endl;
    return true;
  }

  string currency_filter;
  cout << "Введите валюту для фильтрации (например, USD или EUR): ";
  getline(cin, currency_filter);

  if (currency_filter.empty()) {
    cout << "Ошибка: название валюты не может быть пустым!" << endl;
    return true;
  }

  vector<CurrencyRate> filtered;
  for (const auto& rate : rates) {
    if (rate.currency1() == currency_filter ||
        rate.currency2() == currency_filter) {
      filtered.push_back(rate);
    }
  }

  cout << "\n=== Данные для валюты '" << currency_filter << "' ===" << endl;
  cout << "Найдено записей: " << filtered.size() << endl;

  if (filtered.empty()) {
    cout << "Записей для указанной валюты не найдено." << endl;
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
  auto rates = repository->get_all();

  if (rates.empty()) {
    cout << "Нет данных для фильтрации." << endl;
    return true;
  }

  string date_filter;
  cout << "Введите дату для фильтрации (ГГГГ.ММ.ДД): ";
  getline(cin, date_filter);

  try {
    CurrencyRateValidator::validate_date(date_filter);
  } catch (const CurrencyRateException& e) {
    cout << "Ошибка: " << e.what() << endl;
    return true;
  }

  vector<CurrencyRate> filtered;
  for (const auto& rate : rates) {
    if (rate.date() == date_filter) {
      filtered.push_back(rate);
    }
  }

  cout << "\n=== Данные за дату '" << date_filter << "' ===" << endl;
  cout << "Найдено записей: " << filtered.size() << endl;

  if (filtered.empty()) {
    cout << "Записей за указанную дату не найдено." << endl;
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
      cout << "Ошибка: поле не может быть пустым!" << endl;
    } else {
      break;
    }
  }
  return value;
}

double ReadRate() {
  double value;
  while (true) {
    string input = ReadString("Введите курс обмена: ", true);

    try {
      value = stod(input);

      try {
        CurrencyRateValidator::validate_rate(value);
        break;
      } catch (const CurrencyRateException& e) {
        cout << "Ошибка: " << e.what() << endl;
      }
    } catch (const std::invalid_argument&) {
      cout << "Ошибка: введите корректное числовое значение!" << endl;
    } catch (const std::out_of_range&) {
      cout << "Ошибка: число вне допустимого диапазона!" << endl;
    }
  }
  return value;
}

string ReadDate() {
  string date;
  while (true) {
    date = ReadString("Введите дату (ГГГГ.ММ.ДД): ", true);

    try {
      CurrencyRateValidator::validate_date(date);
      break;
    } catch (const CurrencyRateException& e) {
      cout << "Ошибка: " << e.what() << endl;
    }
  }
  return date;
}

bool AddManualData(shared_ptr<ICurrencyRateRepository> repository,
                   const string& filename) {
  cout << "\n=== Ручной ввод данных ===" << endl;

  try {
    string currency1 = ReadString("Введите первую валюту: ", true);
    CurrencyRateValidator::validate_currency_name(currency1);

    string currency2 = ReadString("Введите вторую валюту: ", true);
    CurrencyRateValidator::validate_currency_name(currency2);

    double rate = ReadRate();
    string date = ReadDate();

    CurrencyRate new_data(currency1, currency2, rate, date);
    repository->add(new_data);

    auto memory_repo = std::dynamic_pointer_cast<MemoryCurrencyRateRepository>(
        repository);
    if (memory_repo) {
      memory_repo->append_to_file(filename, new_data);
    }

    cout << "\nДанные успешно добавлены!" << endl;
    cout << "Валюта 1: " << new_data.currency1() << endl;
    cout << "Валюта 2: " << new_data.currency2() << endl;
    cout << std::fixed << setprecision(4)
         << "Курс: " << new_data.rate() << endl;
    cout << "Дата: " << new_data.date() << endl;

  } catch (const CurrencyRateException& e) {
    cout << "Ошибка при добавлении данных: " << e.what() << endl;
  } catch (const std::exception& e) {
    cout << "Неожиданная ошибка: " << e.what() << endl;
  }

  return true;
}

bool SaveToFileMenu(shared_ptr<ICurrencyRateRepository> repository) {
  string filename;
  cout << "Введите имя файла для сохранения: ";
  getline(cin, filename);

  if (filename.empty()) {
    cout << "Ошибка: имя файла не может быть пустым!" << endl;
    return true;
  }

  try {
    auto memory_repo = std::dynamic_pointer_cast<MemoryCurrencyRateRepository>(
        repository);
    if (memory_repo) {
      memory_repo->save_to_file(filename);
      cout << "Данные успешно сохранены в файл: " << filename << endl;
    } else {
      cout << "Ошибка: неподдерживаемая операция" << endl;
    }
  } catch (const std::exception& e) {
    cout << "Ошибка при сохранении файла: " << e.what() << endl;
  }

  return true;
}

bool ExitProgram(shared_ptr<ICurrencyRateRepository> repository) {
  cout << "Выход из программы." << endl;
  return false;
}

}  // namespace

int main() {
  setlocale(LC_ALL, "ru_RU.UTF-8");
  cout << "=== Менеджер курсов валют ===" << endl;

  string filename;
  cout << "Введите имя файла с данными: ";
  getline(cin, filename);

  if (filename.empty()) {
    cout << "Имя файла не указано. "
         << "Будет использован файл по умолчанию: rates.txt" << endl;
    filename = "rates.txt";
  }

  auto parser = make_unique<RegexCurrencyRateParser>();
  auto repository = make_shared<MemoryCurrencyRateRepository>(move(parser));

  try {
    repository->add_from_file(filename);
    cout << "Успешно загружено записей из файла: "
         << repository->count() << endl;
  } catch (const std::exception& e) {
    cout << "Предупреждение: " << e.what() << endl;
    cout << "Начинаем работу с пустой базой данных." << endl;
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
    cout << "\n=== МЕНЮ ===" << endl;
    cout << "1. Показать все курсы" << endl;
    cout << "2. Отсортировать по дате" << endl;
    cout << "3. Отсортировать по валютам" << endl;
    cout << "4. Фильтровать по валюте" << endl;
    cout << "5. Фильтровать по дате" << endl;
    cout << "6. Добавить курс вручную" << endl;
    cout << "7. Сохранить в файл" << endl;
    cout << "8. Выйти" << endl;
    cout << "Выберите действие: ";

    string input;
    getline(cin, input);

    if (input.empty()) {
      cout << "Ошибка: введите число от 1 до 8!" << endl;
      continue;
    }

    try {
      choice = stoi(input);

      if (menu_functions.find(choice) != menu_functions.end()) {
        should_continue = menu_functions.at(choice)();
      } else {
        cout << "Ошибка: неверный пункт меню! Выберите от 1 до 8." << endl;
      }
    } catch (const std::invalid_argument&) {
      cout << "Ошибка: введите корректное число!" << endl;
    } catch (const std::exception& e) {
      cout << "Неожиданная ошибка: " << e.what() << endl;
    }

  } while (should_continue);

  return 0;
}