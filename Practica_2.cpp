// Maslakov Saveliy
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

bool IsEmptyLine(const string& line) {
  return line.empty();
}

bool OpenFile(ifstream& input_file, const string& filename) {
  input_file.open(filename);
  if (!input_file.is_open()) {
    cerr << "Error: Could not open file " << filename << endl;
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

  if (!OpenFile(input_file, filename)) {
    return rates;
  }

  string line;
  while (getline(input_file, line)) {
    ProcessLine(line, rates);
  }

  input_file.close();
  return rates;
}

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

string GetInputFilenameFromUser() {
  cout << "Enter filename: ";
  string filename;
  getline(cin, filename);
  return filename;
}

int main() {
  string filename = GetInputFilenameFromUser();
  vector<CurrencyRate> rates = ReadAllCurrencyRatesFromFile(filename);
  DisplayAllCurrencyRates(rates);
  DisplayCurrencyRatesCount(rates);
  return 0;
}