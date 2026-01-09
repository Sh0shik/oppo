// Maslakov Saveliy
// KI24-07B
// Variant - 15
//

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct CurrencyRate {
  string currency1;  // первая валюта (строка в кавычках)
  string currency2;  // вторая валюта (строка в кавычках)
  double rate;       // курс (дробное число)
  string date;       // дата в формате гггг.мм.дд
};

// ==================== Input/Output Functions ====================

string ReadQuotedCurrencyFromStream(istringstream& iss) {
  iss.get();  // Убираем '"'
  string currency;
  getline(iss, currency, '"');
  return currency;
}

string ReadUnquotedCurrencyFromStream(istringstream& iss) {
  string currency;
  iss >> currency;
  return currency;
}

string ReadCurrencyFromStream(istringstream& iss) {
  iss >> ws;
  char first_char = iss.peek();
  if (first_char == '"') {
    return ReadQuotedCurrencyFromStream(iss);
  }
  return ReadUnquotedCurrencyFromStream(iss);
}

double ReadRateFromStream(istringstream& iss) {
  double rate;
  iss >> rate;
  return rate;
}

string ReadDateFromStream(istringstream& iss) {
  string date;
  iss >> date;
  return date;
}

CurrencyRate ParseCurrencyRate(const string& input_line) {
  istringstream input_stream(input_line);
  CurrencyRate currency_rate;
  currency_rate.currency1 = ReadCurrencyFromStream(input_stream);
  currency_rate.currency2 = ReadCurrencyFromStream(input_stream);
  currency_rate.rate = ReadRateFromStream(input_stream);
  currency_rate.date = ReadDateFromStream(input_stream);
  return currency_rate;
}

// ==================== File Operations ====================

bool IsEmptyLine(const string& line) { return line.empty(); }

bool OpenFileForReading(ifstream& input_file, const string& filename) {
  input_file.open(filename);
  if (!input_file.is_open()) {
    cerr << "Error: Could not open file " << filename << " for reading" << endl;
    return false;
  }
  return true;
}

bool OpenFileForWriting(ofstream& output_file, const string& filename) {
  output_file.open(filename, ios::app);
  if (!output_file.is_open()) {
    cerr << "Error: Could not open file " << filename << " for writing" << endl;
    return false;
  }
  return true;
}

void ProcessLine(const string& line, vector<CurrencyRate>& rates) {
  if (IsEmptyLine(line)) {
    return;
  }

  try {
    CurrencyRate currency_rate = ParseCurrencyRate(line);
    rates.push_back(currency_rate);
  } catch (...) {
    cerr << "Error parsing line: " << line << endl;
  }
}

vector<CurrencyRate> ReadAllCurrencyRatesFromFile(const string& filename) {
  vector<CurrencyRate> rates;
  ifstream input_file;

  if (!OpenFileForReading(input_file, filename)) {
    return rates;
  }

  string line;
  while (getline(input_file, line)) {
    ProcessLine(line, rates);
  }

  input_file.close();
  return rates;
}

void WriteCurrencyRateToFile(ofstream& output_file,
                             const CurrencyRate& currency_rate) {
  // Write first currency (with quotes if contains spaces)
  if (currency_rate.currency1.find(' ') != string::npos) {
    output_file << "\"" << currency_rate.currency1 << "\" ";
  } else {
    output_file << currency_rate.currency1 << " ";
  }

  // Write second currency (with quotes if contains spaces)
  if (currency_rate.currency2.find(' ') != string::npos) {
    output_file << "\"" << currency_rate.currency2 << "\" ";
  } else {
    output_file << currency_rate.currency2 << " ";
  }

  // Write rate and date
  output_file << fixed << setprecision(4) << currency_rate.rate << " "
              << currency_rate.date << endl;
}

bool AppendCurrencyRateToFile(const string& filename,
                              const CurrencyRate& currency_rate) {
  ofstream output_file;
  if (!OpenFileForWriting(output_file, filename)) {
    return false;
  }

  WriteCurrencyRateToFile(output_file, currency_rate);
  output_file.close();
  return true;
}

// ==================== Display Functions ====================

void DisplaySingleCurrencyRate(const CurrencyRate& currency_rate) {
  cout << "Currency 1: " << currency_rate.currency1 << endl;
  cout << "Currency 2: " << currency_rate.currency2 << endl;
  cout << fixed << setprecision(4);
  cout << "Rate: " << currency_rate.rate << endl;
  cout << "Date: " << currency_rate.date << endl;
  cout << "---" << endl;
}

void DisplayAllCurrencyRates(const vector<CurrencyRate>& rates) {
  cout << "\nCurrency rates:" << endl;
  cout << "===============" << endl;

  for (const auto& currency_rate : rates) {
    DisplaySingleCurrencyRate(currency_rate);
  }
}

void DisplayCurrencyRatesCount(const vector<CurrencyRate>& rates) {
  cout << "Total rates: " << rates.size() << endl;
}

// ==================== User Input Functions ====================

string GetInputFilenameFromUser() {
  cout << "Enter filename: ";
  string filename;
  getline(cin, filename);
  return filename;
}

string GetCurrencyInput(const string& prompt) {
  string currency;
  cout << prompt;
  getline(cin, currency);

  // Trim leading/trailing spaces
  size_t start = currency.find_first_not_of(" ");
  size_t end = currency.find_last_not_of(" ");
  if (start != string::npos && end != string::npos) {
    currency = currency.substr(start, end - start + 1);
  }

  return currency;
}

double GetRateInput() {
  double rate;
  while (true) {
    cout << "Enter exchange rate: ";
    string input;
    getline(cin, input);

    try {
      rate = stod(input);
      if (rate <= 0) {
        cout << "Rate must be positive. Please try again." << endl;
        continue;
      }
      break;
    } catch (const exception& e) {
      cout << "Invalid input. Please enter a valid number." << endl;
    }
  }
  return rate;
}

string GetDateInput() {
  string date;
  while (true) {
    cout << "Enter date (YYYY.MM.DD): ";
    getline(cin, date);

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
    cout << "Invalid date format. Please use YYYY.MM.DD format." << endl;
  }
  return date;
}

CurrencyRate GetCurrencyRateFromUser() {
  CurrencyRate new_rate;

  cout << "\n=== Enter new currency rate ===" << endl;
  new_rate.currency1 = GetCurrencyInput("Enter first currency: ");
  new_rate.currency2 = GetCurrencyInput("Enter second currency: ");
  new_rate.rate = GetRateInput();
  new_rate.date = GetDateInput();

  return new_rate;
}

// ==================== Menu Functions ====================

void DisplayMenu() {
  cout << "\n========= Currency Rate Manager =========" << endl;
  cout << "1. View all currency rates" << endl;
  cout << "2. Add new currency rate" << endl;
  cout << "3. Show total count" << endl;
  cout << "4. Exit" << endl;
  cout << "=========================================" << endl;
  cout << "Enter your choice (1-4): ";
}

int GetMenuChoice() {
  string input;
  getline(cin, input);

  try {
    int choice = stoi(input);
    if (choice >= 1 && choice <= 4) {
      return choice;
    }
  } catch (const exception& e) {
    // Invalid input, will be handled by the caller
  }

  return -1; // Invalid choice
}

void HandleAddCurrencyRate(const string& filename, vector<CurrencyRate>& rates) {
  CurrencyRate new_rate = GetCurrencyRateFromUser();

  if (AppendCurrencyRateToFile(filename, new_rate)) {
    rates.push_back(new_rate);
    cout << "\nCurrency rate added successfully!" << endl;
    DisplaySingleCurrencyRate(new_rate);
  } else {
    cout << "\nFailed to save currency rate to file." << endl;
  }
}

// ==================== Main Program ====================

int main() {
  string filename = GetInputFilenameFromUser();
  vector<CurrencyRate> rates = ReadAllCurrencyRatesFromFile(filename);

  if (rates.empty()) {
    cout << "No currency rates found or file is empty." << endl;
  } else {
    cout << "Successfully loaded " << rates.size() << " currency rates." << endl;
  }

  bool running = true;
  while (running) {
    DisplayMenu();
    int choice = GetMenuChoice();

    switch (choice) {
      case 1:  // View all currency rates
        if (rates.empty()) {
          cout << "\nNo currency rates to display." << endl;
        } else {
          DisplayAllCurrencyRates(rates);
        }
        break;

      case 2:  // Add new currency rate
        HandleAddCurrencyRate(filename, rates);
        break;

      case 3:  // Show total count
        DisplayCurrencyRatesCount(rates);
        break;

      case 4:  // Exit
        cout << "\nExiting program. Goodbye!" << endl;
        running = false;
        break;

      default:
        cout << "\nInvalid choice. Please enter a number from 1 to 4." << endl;
        break;
    }
  }

  return 0;
}