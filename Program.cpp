// Maslakov Saveliy
// KI24-07B
// Variant - 15

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct CurrencyRate {
  std::string currency1;  // первая валюта (строка в кавычках)
  std::string currency2;  // вторая валюта (строка в кавычках)
  double rate;            // курс (дробное число)
  std::string date;       // дата в формате гггг.мм.дд
};

// ==================== Sorting Functions ====================
void sort_by_date(std::vector<CurrencyRate>& rates) {
  std::sort(rates.begin(), rates.end(),
            [](const CurrencyRate& a, const CurrencyRate& b) {
              return a.date < b.date;
            });
}

void sort_by_currency_name(std::vector<CurrencyRate>& rates) {
  std::sort(rates.begin(), rates.end(),
            [](const CurrencyRate& a, const CurrencyRate& b) {
              if (a.currency1 != b.currency1) {
                return a.currency1 < b.currency1;
              }
              return a.currency2 < b.currency2;
            });
}

// ==================== Input/Output Functions ====================
CurrencyRate parse_currency_rate(const std::string& input_line) {
  static const std::regex pattern(
      R"(^\s*(\"([^\"]*)\"|([^ \"]+))\s+(\"([^\"]*)\"|([^ \"]+))\s+([\d.]+)\s+(\d{4}\.\d{2}\.\d{2})\s*$)");
  std::smatch matches;
  if (std::regex_match(input_line, matches, pattern)) {
    CurrencyRate currency_rate;
    // Currency1: either group 2 (quoted) or 3 (unquoted)
    currency_rate.currency1 = matches[2].matched ? matches[2].str() : matches[3].str();
    // Currency2: either group 5 (quoted) or 6 (unquoted)
    currency_rate.currency2 = matches[5].matched ? matches[5].str() : matches[6].str();
    currency_rate.rate = std::stod(matches[7].str());
    currency_rate.date = matches[8].str();
    return currency_rate;
  }
  throw std::runtime_error("Invalid line format");
}

// ==================== File Operations ====================
bool is_empty_line(const std::string& line) {
  return line.empty();
}

void process_line(const std::string& line, std::vector<CurrencyRate>& rates) {
  if (is_empty_line(line)) {
    return;
  }
  try {
    CurrencyRate currency_rate = parse_currency_rate(line);
    rates.push_back(currency_rate);
  } catch (...) {
    std::cerr << "Error parsing line: " << line << std::endl;
  }
}

std::vector<CurrencyRate> read_all_currency_rates_from_file(
    const std::string& filename) {
  std::vector<CurrencyRate> rates;
  std::ifstream input_file(filename);
  if (!input_file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << " for reading"
              << std::endl;
    return rates;
  }
  std::string line;
  while (std::getline(input_file, line)) {
    process_line(line, rates);
  }
  input_file.close();
  return rates;
}

void write_currency_rate_to_file(std::ofstream& output_file,
                                 const CurrencyRate& currency_rate) {
  // Write first currency (with quotes if contains spaces)
  if (currency_rate.currency1.find(' ') != std::string::npos) {
    output_file << "\"" << currency_rate.currency1 << "\" ";
  } else {
    output_file << currency_rate.currency1 << " ";
  }
  // Write second currency (with quotes if contains spaces)
  if (currency_rate.currency2.find(' ') != std::string::npos) {
    output_file << "\"" << currency_rate.currency2 << "\" ";
  } else {
    output_file << currency_rate.currency2 << " ";
  }
  // Write rate and date
  output_file << std::fixed << std::setprecision(4) << currency_rate.rate
              << " " << currency_rate.date << std::endl;
}

bool append_currency_rate_to_file(const std::string& filename,
                                  const CurrencyRate& currency_rate) {
  std::ofstream output_file(filename, std::ios::app);
  if (!output_file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << " for writing"
              << std::endl;
    return false;
  }
  write_currency_rate_to_file(output_file, currency_rate);
  output_file.close();
  return true;
}

// ==================== Display Functions ====================
void display_single_currency_rate(const CurrencyRate& currency_rate) {
  std::cout << "Currency 1: " << currency_rate.currency1 << std::endl;
  std::cout << "Currency 2: " << currency_rate.currency2 << std::endl;
  std::cout << std::fixed << std::setprecision(4);
  std::cout << "Rate: " << currency_rate.rate << std::endl;
  std::cout << "Date: " << currency_rate.date << std::endl;
  std::cout << "---" << std::endl;
}

void display_all_currency_rates(const std::vector<CurrencyRate>& rates) {
  std::cout << "\nCurrency rates:" << std::endl;
  std::cout << "===============" << std::endl;
  for (const auto& currency_rate : rates) {
    display_single_currency_rate(currency_rate);
  }
}

void display_currency_rates_count(const std::vector<CurrencyRate>& rates) {
  std::cout << "Total rates: " << rates.size() << std::endl;
}

// ==================== User Input Functions ====================
std::string get_input_filename_from_user() {
  std::cout << "Enter filename: ";
  std::string filename;
  std::getline(std::cin, filename);
  return filename;
}

std::string get_currency_input(const std::string& prompt) {
  std::string currency;
  std::cout << prompt;
  std::getline(std::cin, currency);
  // Trim leading/trailing spaces
  size_t start = currency.find_first_not_of(" ");
  size_t end = currency.find_last_not_of(" ");
  if (start != std::string::npos && end != std::string::npos) {
    currency = currency.substr(start, end - start + 1);
  }
  return currency;
}

double get_rate_input() {
  double rate;
  while (true) {
    std::cout << "Enter exchange rate: ";
    std::string input;
    std::getline(std::cin, input);
    try {
      rate = std::stod(input);
      if (rate <= 0) {
        std::cout << "Rate must be positive. Please try again." << std::endl;
        continue;
      }
      break;
    } catch (const std::invalid_argument&) {
      std::cout << "Invalid input: not a valid number. Please try again."
                << std::endl;
    } catch (const std::out_of_range&) {
      std::cout << "Invalid input: number out of range. Please try again."
                << std::endl;
    }
  }
  return rate;
}

std::string get_date_input() {
  std::string date;
  while (true) {
    std::cout << "Enter date (YYYY.MM.DD): ";
    std::getline(std::cin, date);
    // Simple date format validation
    if (date.length() == 10 && date[4] == '.' && date[7] == '.') {
      bool valid = true;
      for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && !isdigit(date[i])) {
          valid = false;
          break;
        }
      }
      if (valid) {
        break;
      }
    }
    std::cout << "Invalid date format. Please use YYYY.MM.DD format."
              << std::endl;
  }
  return date;
}

CurrencyRate get_currency_rate_from_user() {
  CurrencyRate new_rate;
  std::cout << "\n=== Enter new currency rate ===" << std::endl;
  new_rate.currency1 = get_currency_input("Enter first currency: ");
  new_rate.currency2 = get_currency_input("Enter second currency: ");
  new_rate.rate = get_rate_input();
  new_rate.date = get_date_input();
  return new_rate;
}

// ==================== Menu Functions ====================
void display_menu() {
  std::cout << "\n========= Currency Rate Manager =========" << std::endl;
  std::cout << "1. View all currency rates" << std::endl;
  std::cout << "2. Add new currency rate" << std::endl;
  std::cout << "3. Show total count" << std::endl;
  std::cout << "4. Sort by date" << std::endl;
  std::cout << "5. Sort by currency name" << std::endl;
  std::cout << "6. Exit" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::cout << "Enter your choice (1-6): ";
}

int get_menu_choice() {
  std::string input;
  std::getline(std::cin, input);
  try {
    int choice = std::stoi(input);
    if (choice >= 1 && choice <= 6) {
      return choice;
    }
  } catch (const std::exception&) {
    // Invalid input, will be handled by the caller
  }
  return -1;  // Invalid choice
}

void handle_add_currency_rate(const std::string& filename,
                              std::vector<CurrencyRate>& rates) {
  CurrencyRate new_rate = get_currency_rate_from_user();
  if (append_currency_rate_to_file(filename, new_rate)) {
    rates.push_back(new_rate);
    std::cout << "\nCurrency rate added successfully!" << std::endl;
    display_single_currency_rate(new_rate);
  } else {
    std::cout << "\nFailed to save currency rate to file." << std::endl;
  }
}

// ==================== Main Program ====================
int main() {
  std::string filename = get_input_filename_from_user();
  std::vector<CurrencyRate> rates = read_all_currency_rates_from_file(filename);
  if (rates.empty()) {
    std::cout << "No currency rates found or file is empty." << std::endl;
  } else {
    std::cout << "Successfully loaded " << rates.size() << " currency rates."
              << std::endl;
  }
  bool running = true;
  while (running) {
    display_menu();
    int choice = get_menu_choice();
    static const std::unordered_map<int, std::function<void()>> actions = {
        {1,
         [&rates]() {
           if (rates.empty()) {
             std::cout << "\nNo currency rates to display." << std::endl;
           } else {
             display_all_currency_rates(rates);
           }
         }},
        {2, [&filename, &rates]() { handle_add_currency_rate(filename, rates); }},
        {3, [&rates]() { display_currency_rates_count(rates); }},
        {4,
         [&rates]() {
           sort_by_date(rates);
           std::cout << "\nSorted by date." << std::endl;
         }},
        {5,
         [&rates]() {
           sort_by_currency_name(rates);
           std::cout << "\nSorted by currency name." << std::endl;
         }},
        {6,
         [&running]() {
           std::cout << "\nExiting program. Goodbye!" << std::endl;
           running = false;
         }}};
    auto it = actions.find(choice);
    if (it != actions.end()) {
      it->second();
    } else {
      std::cout << "\nInvalid choice. Please enter a number from 1 to 6."
                << std::endl;
    }
  }
  return 0;
}