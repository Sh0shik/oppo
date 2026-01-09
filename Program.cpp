//Maslakov Saveliy
//KI24-07B

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct CurrencyRate {
  string currency1;    // первая валюта (строка в кавычках)
  string currency2;    // вторая валюта (строка в кавычках)
  double rate;         // курс (дробное число)
  string date;         // дата в формате гггг.мм.дд
};

CurrencyRate ParseCurrencyRate(const string& input) {
  CurrencyRate currency;
  istringstream iss(input);

  // Читаем первую валюту
  iss >> ws;
  char first_char = iss.peek();

  if (first_char == '"') {
    iss.get();
    getline(iss, currency.currency1, '"');
  } else {
    iss >> currency.currency1;
  }

  // Читаем вторую валюту
  iss >> ws;
  first_char = iss.peek();

  if (first_char == '"') {
    iss.get();
    getline(iss, currency.currency2, '"');
  } else {
    iss >> currency.currency2;
  }

  // Читаем курс
  iss >> currency.rate;

  // Читаем дату
  iss >> currency.date;

  return currency;
}

void PrintCurrencyRate(const CurrencyRate& cr) {
  cout << "Currency 1: " << cr.currency1 << endl;
  cout << "Currency 2: " << cr.currency2 << endl;
  cout << fixed << setprecision(4);
  cout << "Rate: " << cr.rate << endl;
  cout << "Date: " << cr.date << endl;
  cout << "---" << endl;
}

vector<CurrencyRate> ReadCurrencyRatesFromFile(const string& filename) {
  vector<CurrencyRate> rates;
  ifstream input_file(filename);

  if (!input_file.is_open()) {
    cerr << "Error: Could not open file " << filename << endl;
    return rates;
  }

  string line;
  while (getline(input_file, line)) {
    if (line.empty()) {
      continue;
    }

    try {
      CurrencyRate cr = ParseCurrencyRate(line);
      rates.push_back(cr);
    } catch (...) {
      cerr << "Error parsing line: " << line << endl;
    }
  }

  input_file.close();
  return rates;
}

int main() {
  cout << "Enter filename: ";
  string filename;
  getline(cin, filename);

  vector<CurrencyRate> rates = ReadCurrencyRatesFromFile(filename);

  cout << "\nCurrency rates:" << endl;
  cout << "===============" << endl;

  for (const auto& cr : rates) {
    PrintCurrencyRate(cr);
  }

  cout << "Total rates: " << rates.size() << endl;

  return 0;
}
