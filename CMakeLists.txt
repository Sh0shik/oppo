cmake_minimum_required(VERSION 3.10)
project(CurrencyRateManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основная программа
add_executable(currency_rate_manager
        src/main.cpp
        src/currency_rate.cpp
        src/currency_rate_parser.cpp
        src/currency_rate_repository.cpp
        src/currency_rate_validator.cpp
)

target_include_directories(currency_rate_manager PRIVATE include)

# Модульные тесты
enable_testing()

find_package(GTest REQUIRED)

add_executable(currency_rate_tests
        tests/test.cpp
        src/currency_rate.cpp
        src/currency_rate_parser.cpp
        src/currency_rate_repository.cpp
        src/currency_rate_validator.cpp
)

target_include_directories(currency_rate_tests PRIVATE include)
target_link_libraries(currency_rate_tests GTest::gtest GTest::gtest_main)

add_test(NAME CurrencyRateTests COMMAND currency_rate_tests)